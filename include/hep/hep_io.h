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
	/* Required when hep_configure() sets auth_key and this hook performs send(). */
	unsigned int transport_encrypted;
} hep_io_hooks_t;

void hep_set_io_hooks(const hep_io_hooks_t *hooks, void *user_data);
const hep_io_hooks_t *hep_get_io_hooks(void **user_data_out);

#endif /* HEP_IO_H_ */
