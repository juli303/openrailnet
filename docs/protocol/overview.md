# Protocol Overview

The OpenRailNet (ORN) protocol defines how nodes communicate across a train consist.

It operates on:
- point-to-point full-duplex links between vehicles
- store-and-forward gateways in each node

Key features:
- packet-based communication
- source/destination addressing
- priority levels
- sequence numbers for reliability
- discovery and topology management

The protocol is designed to be transport-agnostic but is currently intended for:
- RS-485 full duplex links