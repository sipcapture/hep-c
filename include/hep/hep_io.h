/*
 * Optional I/O hooks for external event loops (issue #2).
 *
 * When hep_set_io_hooks() is used, the library does not create sockets or call
 * select(); the embedder owns connect/send and poll integration.
 */
#ifndef HEP_IO_H_
#define HEP_IO_H_

#include <stddef.h>

typedef struct hep_io_hooks {
	/* Return 0 on success, non-zero on failure. */
	int (*send)(void *user_data, const void *buffer, unsigned int length);
	/* Optional: called after repeated send failures (reconnect). */
	int (*reconnect)(void *user_data);
	/*
	 * Set to 1 when send() transmits over TLS (or equivalent). Required when
	 * hep_configure() supplies an auth_key — cleartext hooks are rejected.
	 */
	unsigned int transport_encrypted;
} hep_io_hooks_t;

void hep_set_io_hooks(const hep_io_hooks_t *hooks, void *user_data);
const hep_io_hooks_t *hep_get_io_hooks(void **user_data_out);

#endif /* HEP_IO_H_ */
