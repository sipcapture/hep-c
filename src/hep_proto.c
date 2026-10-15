#include "hep/hep_proto.h"

#include <ctype.h>
#include <strings.h>

uint8_t hep_ip_proto_for_transport(const char *transport)
{
	if (!transport)
		return HEP_PROTO_UDP;

	if (!strcasecmp(transport, "udp"))
		return HEP_PROTO_UDP;
	if (!strcasecmp(transport, "tcp"))
		return HEP_PROTO_TCP;
	if (!strcasecmp(transport, "sctp"))
		return HEP_PROTO_SCTP;
	if (!strcasecmp(transport, "tls"))
		return HEP_IP_PROTO_TLS;
	if (!strcasecmp(transport, "ws"))
		return HEP_IP_PROTO_WS;
	if (!strcasecmp(transport, "wss"))
		return HEP_IP_PROTO_WSS;

	return HEP_PROTO_UDP;
}

const char *hep_transport_name(uint8_t ip_proto)
{
	switch (ip_proto) {
	case HEP_PROTO_UDP:
		return "udp";
	case HEP_PROTO_TCP:
		return "tcp";
	case HEP_PROTO_SCTP:
		return "sctp";
	case HEP_IP_PROTO_TLS:
		return "tls";
	case HEP_IP_PROTO_WS:
		return "ws";
	default:
		return "unknown";
	}
}
