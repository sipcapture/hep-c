#include "hep/hep_config.h"
#include "hep/hep_log.h"

#include <netdb.h>
#include <string.h>

/* Globals consumed by core_hep.c */
extern char *capt_host;
extern char *capt_port;
extern char *capt_proto;
extern char *capt_password;
extern int capt_id;
extern int hep_version;
extern int pl_compress;
extern int usessl;
extern struct addrinfo hints[1];

int hep_configure(const hep_capture_config_t *config)
{
	if (!config)
		return -1;

	if (config->host)
		capt_host = (char *)config->host;
	if (config->port)
		capt_port = (char *)config->port;
	if (config->transport)
		capt_proto = (char *)config->transport;
	if (config->auth_key)
		capt_password = (char *)config->auth_key;

	capt_id = config->capture_id > 0 ? config->capture_id : capt_id;
	if (config->hep_version > 0)
		hep_version = config->hep_version;
	pl_compress = config->compress_payload ? 1 : 0;
	usessl = config->use_ssl ? 1 : 0;

	if (capt_password && capt_password[0] != '\0') {
#ifdef USE_SSL
		if (!usessl) {
			HEP_LOG_WARNING("auth_key set: enabling TLS for HEP transport");
			usessl = 1;
		}
#else
		HEP_LOG_ERR("auth_key requires libhep built with OpenSSL (USE_SSL)");
		return -1;
#endif
	}

	memset(hints, 0, sizeof(hints[0]));
	if (capt_proto && !strcmp(capt_proto, "udp"))
		hints[0].ai_socktype = SOCK_DGRAM;
	else
		hints[0].ai_socktype = SOCK_STREAM;
	hints[0].ai_family = AF_UNSPEC;

	return 0;
}
