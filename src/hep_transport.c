#include "hep/hep_log.h"
#include "hep/hep_portability.h"
#include "hep/hep_transport.h"

#ifdef USE_SSL
#include <openssl/ssl.h>
#endif

extern int sock;
extern int usessl;

#ifdef USE_SSL
extern SSL *ssl;
#endif

int hep_socket_send_plain(const void *buf, unsigned int len)
{
	/*
	 * Cleartext transport.  The HEP auth-key chunk (0x000e) is a shared
	 * secret and is never assembled into the buffers handled here; see
	 * pack_hepv3() in core_hep.c and hep_socket_send_tls() below.  Buffers
	 * built with an auth key are only ever written via the TLS transport.
	 */
	if (send(sock, buf, len, 0) == -1) {
		HEP_LOG_ERR("send error");
		return -1;
	}
	return 0;
}

int hep_socket_send_tls(const void *buf, unsigned int len)
{
#ifdef USE_SSL
	if (!usessl || ssl == NULL) {
		HEP_LOG_ERR("TLS transport is not initialized");
		return -1;
	}
	if (SSL_write(ssl, buf, len) < 0) {
		HEP_LOG_ERR("capture: couldn't write ssl socket");
		return -1;
	}
	return 0;
#else
	HEP_LOG_ERR("TLS support not compiled in");
	return -1;
#endif
}
