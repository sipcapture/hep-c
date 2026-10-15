HEP
===

<b>HEP/EEP Encapsulation Protocol</b>

EEP/HEP: Extensible Encapsulation protocol (“EEP” pronounced “HEPPY”) provides a method to duplicate an IP
datagram to a collector by encapsulating the original datagram and its relative header properties 
(as payload, in form of concatenated chunks) within a new IP datagram transmitted over UDP/TCP/SCTP
connections for remote collection. Encapsulation allows for the original content to be transmitted 
without altering the original IP datagram and header contents and provides flexible allocation of
additional chunks containing additional arbitrary data.

HEP/EEP is currently supported natively in platforms such as Kamailio/SER, OpenSIPS, FreeSWITCH, Asterisk and many more.

This repository provides some example reference implementations for Developers and Integrators.

## Building

Autotools (shared library with SONAME):

```bash
./autogen.sh
./configure
make
sudo make install
```

CMake (alternative):

```bash
cmake -B build && cmake --build build
```

Public headers live under `include/hep/`. Configure capture settings with
`hep_configure()` (no XML/xmlread). Optional `hep_set_io_hooks()` integrates
with an external event loop; `hep_set_log_callback()` replaces stderr logging.

See [docs/TRANSPORT.md](docs/TRANSPORT.md) for TLS/WS/WSS `ip_proto` values.

For any question or comment, please visit http://sipcapture.org or contact support@sipcapture.org

