/*
 * HEP ip_proto (chunk 0x0002) values for SIP transports (issue #6).
 */
#ifndef HEP_PROTO_H_
#define HEP_PROTO_H_

#include <stdint.h>

/* Standard IANA protocol numbers (layer 4). */
enum {
	HEP_PROTO_UDP = 17,
	HEP_PROTO_TCP = 6,
	HEP_PROTO_SCTP = 132
};

/* De-facto HEP transport markers (Kamailio sipcapture / OpenSIPS proto_hep). */
enum {
	HEP_PROTO_TLS = 2,  /* IPPROTO_IDP on Linux */
	HEP_PROTO_WS = 50   /* IPPROTO_ESP on Linux */
};

#define HEP_IP_PROTO_TLS HEP_PROTO_TLS
#define HEP_IP_PROTO_WS  HEP_PROTO_WS
#define HEP_IP_PROTO_WSS HEP_PROTO_TLS

const char *hep_transport_name(uint8_t ip_proto);
uint8_t hep_ip_proto_for_transport(const char *transport);

#endif /* HEP_PROTO_H_ */
