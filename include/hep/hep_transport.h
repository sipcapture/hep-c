#ifndef HEP_TRANSPORT_H_
#define HEP_TRANSPORT_H_

/* Packet buffer includes HEP auth-key chunk (0x000e) built from capt_password. */
#define HEP_SEND_AUTH_CHUNK 0x1u

int hep_socket_send_plain(const void *buf, unsigned int len);
int hep_socket_send_tls(const void *buf, unsigned int len);

#endif /* HEP_TRANSPORT_H_ */
