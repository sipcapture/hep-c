#ifndef HEP_TRANSPORT_H_
#define HEP_TRANSPORT_H_

/*
 * Marks a packet that carries the HEP auth-key chunk (0x000e) derived from
 * capt_password.  Such packets must only be written over TLS.
 */
#define HEP_SEND_AUTH_CHUNK 0x1u

int hep_socket_send_plain(const void *buf, unsigned int len);
int hep_socket_send_tls(const void *buf, unsigned int len);

#endif /* HEP_TRANSPORT_H_ */
