/*
 * $Id$
 *
 *  captagent - Homer capture agent. Modular
 *  Duplicate SIP messages in Homer Encapulate Protocol [HEP] [ipv6 version]
 *
 *  Author: Alexandr Dubovikov <alexandr.dubovikov@gmail.com>
 *  (C) Homer Project 2012 (http://www.sipcapture.org)
 *
 *  Copyright (c) 2010-2016 <Alexandr Dubovikov> 
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the <SIPCAPTURE>. The name of the SIPCAPTURE may not be used to 
 * endorse or promote products derived from this software without specific 
 * prior written permission.

 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#ifndef __FAVOR_BSD
#define __FAVOR_BSD
#endif /* __FAVOR_BSD */

#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#ifdef USE_IPV6
#include <netinet/ip6.h>
#endif /* USE_IPV6 */

#include <pcap.h>

#include "api.h"
#include "core_hep.h"

pthread_t call_thread;   
pthread_mutex_t lock;

#ifdef USE_ZLIB
z_stream strm;
#endif /* USE_ZLIB */

int send_hep_basic (rc_info_t *rcinfo, unsigned char *data, unsigned int len) {

	unsigned char *zipData = NULL;
        int sendzip = 0;

#ifdef USE_ZLIB
        int status = 0;
        unsigned long dlen;

        if(pl_compress && hep_version == 3) {
                //dlen = len/1000+len*len+13;

                dlen = compressBound(len);

                zipData  = malloc(dlen); /* give a little bit memmory */

                /* do compress */
                status = compress( zipData, &dlen, data, len );
                if( status != Z_OK ){
                      fprintf(stderr, "data couldn't be compressed\n");
                      sendzip = 0;
                      if(zipData) free(zipData); /* release */
                }                   
                else {              
                        sendzip = 1;
                        len = dlen;
                }
        }

#endif /* USE_ZLIB */

        switch(hep_version) {
        
            case 3:
		return send_hepv3(rcinfo, sendzip  ? zipData : data , len, sendzip);
                break;
                
            case 2:            
            case 1:        
                return send_hepv2(rcinfo, data, len);                    
                break;
                
            default:
                fprintf(stderr, "Unsupported HEP version [%d]\n", hep_version);                
                break;
        }

	if(zipData) free(zipData);
        
        return 0;
}

int send_hepv3 (rc_info_t *rcinfo, unsigned char *data, unsigned int len, unsigned int sendzip) {

    struct hep_generic *hg=NULL;
    void* buffer;
    unsigned int buflen=0, iplen=0,tlen=0;
    hep_chunk_ip4_t src_ip4, dst_ip4;
#ifdef USE_IPV6
    hep_chunk_ip6_t src_ip6, dst_ip6;    
#endif            
    hep_chunk_t payload_chunk;
    hep_chunk_t authkey_chunk;
    static int errors = 0;

    hg = malloc(sizeof(struct hep_generic));
    memset(hg, 0, sizeof(struct hep_generic));

    /* header set */
    memcpy(hg->header.id, "\x48\x45\x50\x33", 4);

    /* IP proto */
    hg->ip_family.chunk.vendor_id = htons(0x0000);
    hg->ip_family.chunk.type_id   = htons(0x0001);
    hg->ip_family.data = rcinfo->ip_family;
    hg->ip_family.chunk.length = htons(sizeof(hg->ip_family));
    
    /* Proto ID */
    hg->ip_proto.chunk.vendor_id = htons(0x0000);
    hg->ip_proto.chunk.type_id   = htons(0x0002);
    hg->ip_proto.data = rcinfo->ip_proto;
    hg->ip_proto.chunk.length = htons(sizeof(hg->ip_proto));
    

    /* IPv4 */
    if(rcinfo->ip_family == AF_INET) {
        /* SRC IP */
        src_ip4.chunk.vendor_id = htons(0x0000);
        src_ip4.chunk.type_id   = htons(0x0003);
        inet_pton(AF_INET, rcinfo->src_ip, &src_ip4.data);
        src_ip4.chunk.length = htons(sizeof(src_ip4));            
        
        /* DST IP */
        dst_ip4.chunk.vendor_id = htons(0x0000);
        dst_ip4.chunk.type_id   = htons(0x0004);
        inet_pton(AF_INET, rcinfo->dst_ip, &dst_ip4.data);        
        dst_ip4.chunk.length = htons(sizeof(dst_ip4));
        
        iplen = sizeof(dst_ip4) + sizeof(src_ip4); 
    }
#ifdef USE_IPV6
      /* IPv6 */
    else if(rcinfo->ip_family == AF_INET6) {
        /* SRC IPv6 */
        src_ip6.chunk.vendor_id = htons(0x0000);
        src_ip6.chunk.type_id   = htons(0x0005);
        inet_pton(AF_INET6, rcinfo->src_ip, &src_ip6.data);
        src_ip6.chunk.length = htons(sizeof(src_ip6));
        
        /* DST IPv6 */
        dst_ip6.chunk.vendor_id = htons(0x0000);
        dst_ip6.chunk.type_id   = htons(0x0006);
        inet_pton(AF_INET6, rcinfo->dst_ip, &dst_ip6.data);
        dst_ip6.chunk.length = htons(sizeof(dst_ip6));    
        
        iplen = sizeof(dst_ip6) + sizeof(src_ip6);
    }
#endif
        
    /* SRC PORT */
    hg->src_port.chunk.vendor_id = htons(0x0000);
    hg->src_port.chunk.type_id   = htons(0x0007);
    hg->src_port.data = htons(rcinfo->src_port);
    hg->src_port.chunk.length = htons(sizeof(hg->src_port));
    
    /* DST PORT */
    hg->dst_port.chunk.vendor_id = htons(0x0000);
    hg->dst_port.chunk.type_id   = htons(0x0008);
    hg->dst_port.data = htons(rcinfo->dst_port);
    hg->dst_port.chunk.length = htons(sizeof(hg->dst_port));
    
    
    /* TIMESTAMP SEC */
    hg->time_sec.chunk.vendor_id = htons(0x0000);
    hg->time_sec.chunk.type_id   = htons(0x0009);
    hg->time_sec.data = htonl(rcinfo->time_sec);
    hg->time_sec.chunk.length = htons(sizeof(hg->time_sec));
    

    /* TIMESTAMP USEC */
    hg->time_usec.chunk.vendor_id = htons(0x0000);
    hg->time_usec.chunk.type_id   = htons(0x000a);
    hg->time_usec.data = htonl(rcinfo->time_usec);
    hg->time_usec.chunk.length = htons(sizeof(hg->time_usec));
    
    /* Protocol TYPE */
    hg->proto_t.chunk.vendor_id = htons(0x0000);
    hg->proto_t.chunk.type_id   = htons(0x000b);
    hg->proto_t.data = rcinfo->proto_type;
    hg->proto_t.chunk.length = htons(sizeof(hg->proto_t));
    
    /* Capture ID */
    hg->capt_id.chunk.vendor_id = htons(0x0000);
    hg->capt_id.chunk.type_id   = htons(0x000c);
    hg->capt_id.data = htons(capt_id);
    hg->capt_id.chunk.length = htons(sizeof(hg->capt_id));

    /* Payload */
    payload_chunk.vendor_id = htons(0x0000);
    payload_chunk.type_id   = sendzip ? htons(0x0010) : htons(0x000f);
    payload_chunk.length    = htons(sizeof(payload_chunk) + len);
    
    tlen = sizeof(struct hep_generic) + len + iplen + sizeof(hep_chunk_t);

    /* auth key */
    if(capt_password != NULL) {

          tlen += sizeof(hep_chunk_t);
          /* Auth key */
          authkey_chunk.vendor_id = htons(0x0000);
          authkey_chunk.type_id   = htons(0x000e);
          authkey_chunk.length    = htons(sizeof(authkey_chunk) + strlen(capt_password));
          tlen += strlen(capt_password);
    }

    /* total */
    hg->header.length = htons(tlen);

    //fprintf(stderr, "LEN: [%d] vs [%d] = IPLEN:[%d] LEN:[%d] CH:[%d]\n", hg->header.length, ntohs(hg->header.length), iplen, len, sizeof(struct hep_chunk));

    buffer = (void*)malloc(tlen);
    if (buffer==0){
        fprintf(stderr,"ERROR: out of memory\n");
        free(hg);
        return 1;
    }
    
    memcpy((void*) buffer, hg, sizeof(struct hep_generic));
    buflen = sizeof(struct hep_generic);

    /* IPv4 */
    if(rcinfo->ip_family == AF_INET) {
        /* SRC IP */
        memcpy((void*) buffer+buflen, &src_ip4, sizeof(struct hep_chunk_ip4));
        buflen += sizeof(struct hep_chunk_ip4);
        
        memcpy((void*) buffer+buflen, &dst_ip4, sizeof(struct hep_chunk_ip4));
        buflen += sizeof(struct hep_chunk_ip4);
    }
#ifdef USE_IPV6
      /* IPv6 */
    else if(rcinfo->ip_family == AF_INET6) {
        /* SRC IPv6 */
        memcpy((void*) buffer+buflen, &src_ip6, sizeof(struct hep_chunk_ip6));
        buflen += sizeof(struct hep_chunk_ip6);
        
        memcpy((void*) buffer+buflen, &dst_ip6, sizeof(struct hep_chunk_ip6));
        buflen += sizeof(struct hep_chunk_ip6);
    }
#endif

    /* AUTH KEY CHUNK */
    if(capt_password != NULL) {

        memcpy((void*) buffer+buflen, &authkey_chunk,  sizeof(struct hep_chunk));
        buflen += sizeof(struct hep_chunk);

        /* Now copying payload self */
        memcpy((void*) buffer+buflen, capt_password, strlen(capt_password));
        buflen+=strlen(capt_password);
    }

    /* PAYLOAD CHUNK */
    memcpy((void*) buffer+buflen, &payload_chunk,  sizeof(struct hep_chunk));
    buflen +=  sizeof(struct hep_chunk);            

    /* Now copying payload self */
    memcpy((void*) buffer+buflen, data, len);    
    buflen+=len;    

    /* make sleep after 100 errors */
     if(errors > 50) {
        fprintf(stderr, "HEP server is down... retrying after sleep...\n");
	if(!usessl) {
	     sleep(2);
             if(init_hepsocket_blocking()) { 
				initfails++; 	
	     	     }
	     	     errors=0;
        }
#ifdef USE_SSL
        else {
		sleep(2);
		 if(initSSL()) {
	 	  	initfails++;
	    		}
	    		errors=0;
       	 }
#endif /* USE SSL */

     }

    /* send this packet out of our socket */
    if(send_data(buffer, buflen)) {
        errors++;    
    }

    /* FREE */        
    if(buffer) free(buffer);
    if(hg) free(hg);        
    
    return 1;
}


int send_hepv2 (rc_info_t *rcinfo, unsigned char *data, unsigned int len) {

    void* buffer;            
    struct hep_hdr hdr;
    struct hep_timehdr hep_time;
    struct hep_iphdr hep_ipheader;
    unsigned int totlen=0, buflen=0;
    static int errors=0;
#ifdef USE_IPV6
    struct hep_ip6hdr hep_ip6header;
#endif /* USE IPV6 */

    /* Version && proto */
    hdr.hp_v = hep_version;
    hdr.hp_f = rcinfo->ip_family;
    hdr.hp_p = rcinfo->ip_proto;
    hdr.hp_sport = htons(rcinfo->src_port); /* src port */
    hdr.hp_dport = htons(rcinfo->dst_port); /* dst port */

    /* IP version */    
    switch (hdr.hp_f) {        
                case AF_INET:
                    totlen  = sizeof(struct hep_iphdr);
                    break;
#ifdef USE_IPV6                    
                case AF_INET6:
                    totlen = sizeof(struct hep_ip6hdr);
                    break;
#endif /* USE IPV6 */
                    
    }
    
    hdr.hp_l = totlen + sizeof(struct hep_hdr);
    
    /* COMPLETE LEN */
    totlen += sizeof(struct hep_hdr);
    totlen += len;

    if(hep_version == 2) {
    	totlen += sizeof(struct hep_timehdr);
        hep_time.tv_sec = rcinfo->time_sec;
        hep_time.tv_usec = rcinfo->time_usec;
        hep_time.captid = capt_id;
    }

    /*buffer for ethernet frame*/
    buffer = (void*)malloc(totlen);
    if (buffer==0){
    	fprintf(stderr,"ERROR: out of memory\n");
        goto error;
    }

    /* copy hep_hdr */
    memcpy((void*) buffer, &hdr, sizeof(struct hep_hdr));
    buflen = sizeof(struct hep_hdr);

    switch (hdr.hp_f) {

    	case AF_INET:
        	/* Source && Destination ipaddresses*/
        	inet_pton(AF_INET, rcinfo->src_ip, &hep_ipheader.hp_src);
        	inet_pton(AF_INET, rcinfo->dst_ip, &hep_ipheader.hp_dst);

                /* copy hep ipheader */
                memcpy((void*)buffer + buflen, &hep_ipheader, sizeof(struct hep_iphdr));
                buflen += sizeof(struct hep_iphdr);

                break;
#ifdef USE_IPV6
	case AF_INET6:

                inet_pton(AF_INET6, rcinfo->src_ip, &hep_ip6header.hp6_src);
                inet_pton(AF_INET6, rcinfo->dst_ip, &hep_ip6header.hp6_dst);                        

                /* copy hep6 ipheader */
                memcpy((void*)buffer + buflen, &hep_ip6header, sizeof(struct hep_ip6hdr));
                buflen += sizeof(struct hep_ip6hdr);
                break;
#endif /* USE_IPV6 */
     }

     /* Version 2 has timestamp, captnode ID */
     if(hep_version == 2) {
     	/* TIMING  */
        memcpy((void*)buffer + buflen, &hep_time, sizeof(struct hep_timehdr));
        buflen += sizeof(struct hep_timehdr);
     }

     memcpy((void *)(buffer + buflen) , (void*)(data), len);
     buflen +=len;

     /* make sleep after 100 errors*/
     if(errors > 50) {
        fprintf(stderr, "HEP server is down... retrying after sleep...\n");
	if(!usessl) {
	     sleep(2);
             if(init_hepsocket_blocking()) { 
				initfails++;
	     	     }
	     	     errors=0;
        }
#ifdef USE_SSL
        else {
	    sleep(2);
	    	    if(initSSL()) {
				initfails++;  
	    	    }
	    	    errors=0;
        }
#endif /* USE SSL */

     }

     /* send this packet out of our socket */
     if(send_data(buffer, buflen)) {
             errors++;    
     }

     /* FREE */
     if(buffer) free(buffer);

     return 1;

error:
     if(buffer) free(buffer);
     return 0;                     
}


int send_data (void *buf, unsigned int len) {

	/* send this packet out of our socket */
	//int r = 0;
	void * p = buf;
	//int sentbytes = 0;

	if(!usessl) {
	        	if(send(sock, p, len, 0) == -1) {
	    	        	printf("send error\n");
            			return -1;
	        	}
          	sendPacketsCount++;
	  /* while (sentbytes < len){
	        	if( (r = send(sock, p, len - sentbytes, MSG_NOSIGNAL )) == -1) {
	    	        	printf("send error\n");
        			return -1;
	        	}
	        	if (r != len - sentbytes)
			    printf("send:multiple calls: %d\n", r);

        		sentbytes += r;
	        	p += r;
        	}
        	sendPacketsCount++;
	  */
        }
#ifdef USE_SSL
        else {
            if(SSL_write(ssl, buf, len) < 0) {            
		fprintf(stderr,"capture: couldn't re-init ssl socket\r\n");
                return -1;                
            }
	    sendPacketsCount++;
        }
#endif        
	/* RESET ERRORS COUNTER */
	return 0;
}


void  select_loop (void)
{
	int n = 0;
	int initfails = 0;
	fd_set readfd;
	time_t prevtime = time(NULL);
	
	
	FD_ZERO(&readfd);
	FD_SET(sock, &readfd);
	while (1){
		if (select(sock+1, &readfd, 0, 0, NULL) < 0){
			perror("select failed\n");
			handler(1);
		}
		if (FD_ISSET(sock, &readfd)){
			ioctl(sock, FIONREAD, &n);
			if (n == 0){
				/* server disconnected*/
		         if(!usessl) {
                   if(init_hepsocket()) initfails++;                                
             }
#ifdef USE_SSL             
             else {
                  if(initSSL()) initfails++;
             }
#endif /* USE_SSL */             

		        if (initfails > 10)
		        {
		        	time_t curtime = time (NULL);
		        	if (curtime - prevtime < 2){
		        		pthread_mutex_lock(&lock);
		        		fprintf(stderr, "HEP server is down... retrying after sleep...\n");
		        		sleep(2);
		        		pthread_mutex_unlock(&lock);
		        	}
		            initfails=0;
		            prevtime = curtime;
		        }
			}
		}
	}
}


int init_hepsocket (void) {

    struct timeval tv; 
    socklen_t lon;
    long arg;
    fd_set myset;
    int valopt, res, ret = 0, s;

    if(sock) close(sock);

    if ((s = getaddrinfo(capt_host, capt_port, hints, &ai)) != 0) {            
            fprintf(stderr, "capture: getaddrinfo: %s\n", gai_strerror(s));
            return 2;
    }

    if((sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
             fprintf(stderr,"Sender socket creation failed: %s\n", strerror(errno));
             return 1;
    }

    // Set non-blocking 
    if((arg = fcntl(sock, F_GETFL, NULL)) < 0) { 
        fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
        close(sock);        
        return 1;
    } 
    arg |= O_NONBLOCK; 
    if( fcntl(sock, F_SETFL, arg) < 0) { 
        fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
        close(sock);        
        return 1; 
    }        

    if((res = connect(sock, ai->ai_addr, (socklen_t)(ai->ai_addrlen))) < 0) {
	if (errno == EINPROGRESS) { 
	        do { 
	           tv.tv_sec = 5; 
	           tv.tv_usec = 0; 
        	   FD_ZERO(&myset); 
	           FD_SET(sock, &myset); 

        	   res = select(sock + 1 , NULL, &myset, NULL, &tv); 
           
	           if (res < 0 && errno != EINTR) { 
        	      fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno)); 
		      close(sock); 
		      ret = 1;
		      break;
	           } 
        	   else if (res > 0) { 
        	      // Socket selected for write 
              
	              lon = sizeof(int); 
        	      if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) { 
			 close(sock); 
        	         fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno)); 
        	         ret = 2;
	              } 	
        	      // Check the value returned... 
	              if (valopt) { 
			 close(sock); 
	                 fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt)); 
	                 ret = 3;
        	      } 
	              break; 
	           } 
        	   else { 
		      close(sock); 
	              fprintf(stderr, "Timeout in select() - Cancelling!\n"); 
	              ret = 4; 
	              break;
	           } 
        	} while (1); 
	}
    }

    return ret;
}

int init_hepsocket_blocking (void) {

    int s;
    struct timeval tv;
    fd_set myset;

    if(sock) close(sock);

    if ((s = getaddrinfo(capt_host, capt_port, hints, &ai)) != 0) {            
            fprintf(stderr, "capture: getaddrinfo: %s\n", gai_strerror(s));
            return 2;
    }

    if((sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
             fprintf(stderr,"Sender socket creation failed: %s\n", strerror(errno));
             return 1;
    }

     if (connect(sock, ai->ai_addr, (socklen_t)(ai->ai_addrlen)) == -1) {
         select(sock + 1 , NULL, &myset, NULL, &tv);
         if (errno != EINPROGRESS) {
             fprintf(stderr,"Sender socket creation failed: %s\n", strerror(errno));
             return 1;    
          }
    }


    return 0;
}



#ifdef USE_SSL
SSL_CTX* initCTX(void) {
        SSL_METHOD *method;
        SSL_CTX *ctx;

        OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
        SSL_load_error_strings();   /* Bring in and register error messages */

        /* we use SSLv3 */
        method = SSLv3_client_method();  /* Create new client-method instance */

        ctx = SSL_CTX_new(method);   /* Create new context */
        if ( ctx == NULL ) {
                ERR_print_errors_fp(stderr);
                abort();
        }
        return ctx;
}
 
 
void showCerts(SSL* ssl) {
        
        X509 *cert;
        char *line;

        cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
        if ( cert != NULL ) {
                fprintf(stderr,"Server certificates:\n");
                line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
                fprintf(stderr,"Subject: %s\n", line);
                free(line);       /* free the malloc'ed string */
                line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
                fprintf(stderr,"Issuer: %s\n", line);
                free(line);       /* free the malloc'ed string */
                X509_free(cert);     /* free the malloc'ed certificate copy */
        }
        else
                fprintf(stderr,"No certificates.\n");
}

int initSSL(void) {

        long ctx_options;

        /* if(ssl) SSL_free(ssl);
        if(ctx) SSL_CTX_free(ctx);
        */

        if(init_hepsocket_blocking()) {
                fprintf(stderr,"capture: couldn't init hep socket\r\n");
                return 1;
        }


        ctx = initCTX();

        /* workaround bug openssl */
        ctx_options = SSL_OP_ALL;   
        ctx_options |= SSL_OP_NO_SSLv2;
        SSL_CTX_set_options(ctx, ctx_options);
                
        /*extra*/
        SSL_CTX_ctrl(ctx, BIO_C_SET_NBIO, 1, NULL);

        /* create new SSL connection state */
        ssl = SSL_new(ctx);

        SSL_set_connect_state(ssl);

        /* attach socket */
        SSL_set_fd(ssl, sock);    /* attach the socket descriptor */
                
        /* perform the connection */
        if ( SSL_connect(ssl) == -1 )  {
              ERR_print_errors_fp(stderr);
              return 1;
        }                 
                          
        showCerts(ssl);   

        return 0;
}

#endif /* use SSL */


char *description(void)
{
        printf("Loaded description\n");
        char *description = "test description";
        
        return description;
}

int statistic(char *buf)
{
        snprintf(buf, 1024, "Statistic of CORE_HEP module:\r\nSend packets: [%i]\r\n", sendPacketsCount);
        return 1;
}

int handlerPipe(void) {

        printf("SIGPIPE... trying to reconnect...\n");
        return 1;
}


int sigPipe(void)
{

        struct sigaction new_action;

        /* sigation structure */
        new_action.sa_handler = handlerPipe;
        sigemptyset (&new_action.sa_mask);
        new_action.sa_flags = 0;

        if( sigaction (SIGPIPE, &new_action, NULL) == -1) {
                perror("Failed to set new Handle");
                return -1;
        }

}

