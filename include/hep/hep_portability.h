/*
 * Cross-platform networking includes (issue #8).
 */
#ifndef HEP_PORTABILITY_H_
#define HEP_PORTABILITY_H_

#if defined(_WIN32) || defined(__CYGWIN__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int hep_socklen_t;
#ifndef HEP_HAVE_NETDB
#define HEP_HAVE_NETDB 1
#endif
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
typedef socklen_t hep_socklen_t;
#ifndef HEP_HAVE_NETDB
#define HEP_HAVE_NETDB 1
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#define HEP_PACKED __attribute__((packed))
#else
#define HEP_PACKED
#endif

#endif /* HEP_PORTABILITY_H_ */
