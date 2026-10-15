# HEP transport identifiers (`ip_proto` chunk)

The HEP/EEP `ip_proto` chunk (type `0x0002`) carries an 8-bit protocol number.
For plain UDP, TCP, and SCTP this is the usual `IPPROTO_*` value from the OS.

TLS, WebSocket (`ws`), and secure WebSocket (`wss`) are not IP protocols. SIP
stacks therefore use reserved numbers so capture nodes can tell them apart from
raw TCP:

| Application transport | Suggested `ip_proto` | Notes |
|----------------------|----------------------|-------|
| UDP | 17 (`IPPROTO_UDP`) | Default SIP/UDP |
| TCP | 6 (`IPPROTO_TCP`) | SIP/TCP |
| SCTP | 132 (`IPPROTO_SCTP`) | |
| TLS | 2 (`IPPROTO_IDP`) | De-facto marker in Kamailio/OpenSIPS |
| WS | 50 (`IPPROTO_ESP`) | Used by OpenSIPS `proto_hep` |
| WSS | 2 (`IPPROTO_IDP`) or 6 | Often same as TLS; payload is still SIP |

Use `hep_ip_proto_for_transport()` from `hep_proto.h` when mapping stack
transport names. The SIP/SDP payload type remains in the `proto_t` chunk
(`0x000b`), not in `ip_proto`.

FreeSWITCH historically mapped `tls` to `IPPROTO_IDP` and `ws`/`wss` to
`IPPROTO_TCP`; that loses transport detail on the Homer side. Prefer the values
above for new integrations.
