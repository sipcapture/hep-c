/*
 * Public HEP capture API.
 */
#ifndef HEP_API_H_
#define HEP_API_H_

#include <stdint.h>

#include "hep_config.h"
#include "hep_io.h"
#include "hep_log.h"
#include "hep_proto.h"

struct rc_info {
	uint8_t ip_family;
	uint8_t ip_proto;
	uint8_t proto_type;
	char *src_mac;
	char *dst_mac;
	char *src_ip;
	char *dst_ip;
	uint16_t src_port;
	uint16_t dst_port;
	uint32_t time_sec;
	uint32_t time_usec;
	uint32_t liid;
	uint32_t cval1;
	uint32_t cval2;
	uint16_t sessionid;
	uint8_t direction;
	char *uuid;
};

typedef struct rc_info rc_info_t;

/* Encode and send using configured transport or I/O hooks. */
int send_hep_basic(rc_info_t *rcinfo, unsigned char *data, unsigned int len);

/*
 * Encode only: caller sends bytes via hep_io_hooks or another path.
 * On success, *out_buf receives a malloc()'d packet and *out_len its size.
 */
int hep_build_packet(rc_info_t *rcinfo, const unsigned char *data, unsigned int len,
                     unsigned char **out_buf, unsigned int *out_len);

int hep_send_buffer(const void *buffer, unsigned int length);

/* Built-in blocking/non-blocking socket helpers (optional). */
int init_hepsocket(void);
int init_hepsocket_blocking(void);
void select_loop(void);

int statistic(char *buf);

#endif /* HEP_API_H_ */
