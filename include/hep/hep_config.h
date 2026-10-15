/*
 * Runtime configuration without XML / xmlread (issue #7).
 */
#ifndef HEP_CONFIG_H_
#define HEP_CONFIG_H_

typedef struct hep_capture_config {
	const char *host;
	const char *port;
	/* "udp", "tcp", "tls", "sctp", "ws", "wss" — see hep_proto.h */
	const char *transport;
	/* Requires use_ssl=1 (built-in transport) or encrypted I/O hooks. */
	const char *auth_key;
	int capture_id;
	int hep_version;
	int compress_payload;
	int use_ssl;
} hep_capture_config_t;

/* Apply settings used by the built-in socket transport (ignored when I/O hooks are set). */
int hep_configure(const hep_capture_config_t *config);

#endif /* HEP_CONFIG_H_ */
