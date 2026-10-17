/*
 * Regression test for code scanning alert cpp/cleartext-transmission
 * (src/hep_transport.c): the HEP auth-key chunk (0x000e) must never be
 * written to the cleartext transport, and packets that carry it must be
 * handed to the TLS transport only.
 *
 * The test exercises two things:
 *   1. With an auth key configured and TLS disabled, send_data() refuses to
 *      transmit instead of falling back to cleartext send().
 *   2. hep_build_packet() still assembles a well-formed HEPv3 packet that
 *      contains the auth-key chunk and whose header length covers it.
 */
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "hep/hep_api.h"
#include "hep/hep_transport.h"

/* Internal symbols exported by libhep (see core_hep.c). */
extern int sock;
extern int usessl;
extern char *capt_password;
extern int send_data(void *buf, unsigned int len, unsigned int send_flags);

/* libhep calls this embedder-provided hook; the test only needs a stub. */
void handler(int value)
{
    (void)value;
}

static int failures = 0;

#define CHECK(cond, msg)                                                       \
    do {                                                                       \
        if (!(cond)) {                                                         \
            fprintf(stderr, "FAIL: %s (%s:%d)\n", (msg), __FILE__, __LINE__);  \
            failures++;                                                        \
        }                                                                      \
    } while (0)

static const unsigned char *find_bytes(const unsigned char *hay, size_t hay_len,
                                       const unsigned char *needle,
                                       size_t needle_len)
{
    size_t i;

    if (needle_len == 0 || hay_len < needle_len)
        return NULL;
    for (i = 0; i + needle_len <= hay_len; i++) {
        if (memcmp(hay + i, needle, needle_len) == 0)
            return hay + i;
    }
    return NULL;
}

static int drain(int fd, char *out, size_t cap)
{
    ssize_t n = recv(fd, out, cap, MSG_DONTWAIT);
    if (n < 0)
        return (errno == EAGAIN || errno == EWOULDBLOCK) ? 0 : -1;
    return (int)n;
}

static void test_cleartext_refused(void)
{
    int sv[2];
    const char *key = "s3cr3t-hep-auth-key";
    char buf[64];
    int n;

    CHECK(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0, "socketpair");
    sock = sv[0];
    usessl = 0;
    capt_password = (char *)key;

    /* Auth flagged explicitly: must be refused, not sent in cleartext. */
    CHECK(send_data((void *)key, (unsigned int)strlen(key),
                    HEP_SEND_AUTH_CHUNK) == -1,
          "auth-key packet must be refused when TLS is disabled");

    /* Auth configured implicitly: must also be refused. */
    CHECK(send_data((void *)key, (unsigned int)strlen(key), 0) == -1,
          "active auth_key must force TLS, never cleartext");

    n = drain(sv[1], buf, sizeof(buf));
    CHECK(n == 0, "no auth-key bytes may reach the cleartext socket");

    /* Without an auth key the cleartext transport is still allowed. */
    capt_password = NULL;
    CHECK(send_data((void *)"plainhep", 8, 0) == 0, "plain send allowed");
    n = drain(sv[1], buf, sizeof(buf));
    CHECK(n == 8 && memcmp(buf, "plainhep", 8) == 0,
          "plain payload delivered over cleartext");

    close(sv[0]);
    close(sv[1]);
}

static void test_packet_contains_auth_chunk(void)
{
    static const char key[] = "s3cr3t-hep-auth-key";
    static const unsigned char payload[] = "INVITE sip:x HEP payload";
    rc_info_t rc;
    unsigned char *pkt = NULL;
    unsigned int pktlen = 0;
    const unsigned char *at;
    const unsigned char *pl;
    size_t ko;
    unsigned int chunk_len;
    unsigned int hdr_len;

    memset(&rc, 0, sizeof(rc));
    rc.ip_family = AF_INET;
    rc.ip_proto = 17;
    rc.proto_type = 1;
    rc.src_ip = (char *)"10.0.0.1";
    rc.dst_ip = (char *)"10.0.0.2";
    rc.src_port = 5060;
    rc.dst_port = 5060;
    rc.time_sec = 1;
    rc.time_usec = 2;

    capt_password = (char *)key;
    usessl = 1;

    CHECK(hep_build_packet(&rc, payload, (unsigned int)(sizeof(payload) - 1),
                           &pkt, &pktlen) == 0,
          "hep_build_packet succeeds");

    if (pkt == NULL)
        return;

    hdr_len = ((unsigned int)pkt[4] << 8) | pkt[5];
    CHECK(hdr_len == pktlen, "HEP header length covers the whole packet");

    at = find_bytes(pkt, pktlen, (const unsigned char *)key, strlen(key));
    CHECK(at != NULL, "auth key present in the built packet");
    if (at != NULL) {
        ko = (size_t)(at - pkt);
        CHECK(ko >= 6, "auth-key chunk header present");
        /* vendor_id == 0x0000, type_id == 0x000e, length includes header. */
        CHECK(pkt[ko - 6] == 0 && pkt[ko - 5] == 0, "auth vendor id");
        CHECK(pkt[ko - 4] == 0 && pkt[ko - 3] == 0x0e, "auth chunk type 0x000e");
        chunk_len = ((unsigned int)pkt[ko - 2] << 8) | pkt[ko - 1];
        CHECK(chunk_len == 6 + strlen(key), "auth chunk length");
    }

    pl = find_bytes(pkt, pktlen, payload, sizeof(payload) - 1);
    CHECK(pl != NULL, "payload present in the built packet");
    CHECK(pl == NULL || (at != NULL && pl > at),
          "payload chunk follows the auth-key chunk");

    free(pkt);
}

int main(void)
{
    test_cleartext_refused();
    test_packet_contains_auth_chunk();

    if (failures) {
        fprintf(stderr, "%d check(s) failed\n", failures);
        return 1;
    }
    printf("all auth-transport checks passed\n");
    return 0;
}