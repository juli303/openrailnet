# OpenRailNet Protocol Specification (Draft v0.1)

## Status

Draft / Experimental

This specification is not yet stable. Breaking changes are expected.

---

## 1. Overview

OpenRailNet (ORN) defines a packet-based communication protocol for modular rail vehicles connected in a linear topology.

Each vehicle contains a node that communicates with neighboring nodes via two links:

- Port A
- Port B

Nodes form a chain:

Node ↔ Node ↔ Node ↔ ...

The protocol enables:

- automatic topology discovery
- consist (train) ordering
- distributed control
- multi-node coordination

---

## 2. Design Principles

The protocol follows these principles:

- Deterministic behavior
- Explicit state transitions
- Minimal assumptions about hardware
- Store-and-forward communication
- No implicit master (but supports leadership roles)
- Robust against dynamic topology changes

---

## 3. Network Model

### 3.1 Topology

The network is a **linear chain** of nodes.

Each node has:
- 0–2 neighbors
- one link per side (A and B)

### 3.2 Communication Model

- Full-duplex point-to-point links
- Packet-based transport
- Store-and-forward routing
- No shared bus arbitration required

---

## 4. Node Model

Each node must provide:

- unique `vehicle_id`
- `vehicle_type`
- `front_side` (A or B)
- link interfaces (Port A and Port B)

Each node is responsible for:
- forwarding packets
- detecting neighbors
- participating in discovery
- executing local actions

---

## 5. Packet Format

### 5.1 Frame Structure

Each packet consists of:

- Preamble (2 bytes)
- Header
- Payload (variable length)
- CRC16 (2 bytes)

### 5.2 Preamble

| Field | Size | Value |
|------|------|------|
| PRE1 | 1 byte | 0x55 |
| PRE2 | 1 byte | 0xD5 |

---

### 5.3 Header

| Field        | Size | Description |
|-------------|------|-------------|
| VERFLG      | 1    | Version + flags |
| TYPE        | 1    | Packet type |
| PRIORITY    | 1    | Priority level |
| SRC         | 2    | Source node ID |
| DST         | 2    | Destination node ID |
| SEQ         | 1    | Sequence number |
| LEN         | 1    | Payload length |

---

### 5.4 Version + Flags

Upper 4 bits: protocol version  
Lower 4 bits: flags

Flags:

- bit 0: ACK requested
- bit 1: ACK packet
- bit 2: forwarded packet
- bit 3: reserved

---

### 5.5 Priority Levels

| Value | Meaning |
|------|--------|
| 0 | Emergency |
| 1 | Control |
| 2 | Normal |
| 3 | Diagnostic |

---

### 5.6 Addressing

- `SRC`: sender node ID
- `DST`: receiver node ID

Special values:

- `0xFFFF`: broadcast

---

### 5.7 Sequence Number

- 8-bit counter
- used for:
  - ACK matching
  - duplicate detection
  - retransmission

---

### 5.8 CRC

- CRC16 over header + payload
- excludes preamble

---

## 6. Packet Types

### 6.1 HELLO

Used for neighbor detection.

Payload:

- vehicle_id
- vehicle_type
- front_side
- capabilities
- protocol version

---

### 6.2 HELLO_ACK

Response to HELLO.

Payload:

- acknowledged sequence
- optional status flags

---

### 6.3 DISCOVER_TRIGGER

Initiates discovery.

- sent in one direction only
- used to locate a train end

---

### 6.4 DISCOVER_AUTH

Authoritative discovery packet.

- initiated only by end node
- accumulates node records

Each node appends:

- vehicle_id
- vehicle_type
- front_side
- capabilities

---

### 6.5 CONSIST_TABLE

Final topology result.

Contains:

- ordered list of nodes
- position per node
- orientation per node

---

### 6.6 TRAIN_COMMAND

Control packet.

Payload:

- direction (forward / reverse / hold)
- speed command
- brake command
- flags

---

### 6.7 EMERGENCY_STOP

Immediate stop command.

- broadcast
- highest priority

---

### 6.8 STATUS (Vehicle / Loco)

Provides runtime information.

Examples:
- battery level
- temperature
- fault flags
- current speed

---

### 6.9 ACK / NACK

Used for reliable communication.

Payload:

- acknowledged type
- acknowledged sequence
- status code

---

## 7. Link Management

### 7.1 Keepalive

Each node must:

- send periodic HELLO packets
- monitor incoming traffic per port

### 7.2 Liveness Detection

A port is considered alive if:

- valid packets are received within timeout

Timeout example:
- HELLO interval: 200 ms
- timeout: 700 ms

---

### 7.3 Neighbor State

Per port:

- UNKNOWN
- ALIVE
- LOST

Transitions based on packet reception.

---

## 8. Topology Detection

### 8.1 End Detection

A node is an end node if:

- exactly one port is alive

---

### 8.2 Discovery Flow

1. Node sends DISCOVER_TRIGGER
2. Packet propagates until reaching end node
3. End node starts DISCOVER_AUTH
4. Packet traverses entire chain
5. Each node appends its data
6. Far end generates CONSIST_TABLE
7. Table propagates back to all nodes

---

### 8.3 Topology Changes

Detected via:

- neighbor appearance
- neighbor loss
- neighbor ID change

Triggers re-discovery after stabilization delay.

---

## 9. Routing

### 9.1 Forwarding Rules

Upon receiving a packet:

1. Validate packet (CRC, length)
2. If destination matches → process locally
3. If broadcast → process + forward
4. Otherwise → forward

Forwarding direction:

- A → B
- B → A

---

### 9.2 Duplicate Handling

Nodes must detect duplicates using:

- SRC + TYPE + SEQ

Duplicate packets:
- must not be executed twice
- should still be ACKed if required

---

## 10. Reliability

### 10.1 ACK Behavior

Packets may request ACK.

- sender retransmits if no ACK
- retries use same SEQ

---

### 10.2 Retransmission

- triggered by timeout
- limited retry count recommended

---

## 11. Timing (Recommended)

| Parameter | Value |
|----------|------|
| HELLO interval | 200 ms |
| Alive timeout | 700 ms |
| Topology stabilization | 500–1000 ms |

---

## 12. Transport Layer

The protocol is transport-agnostic.

Reference implementation uses:

- RS-485 full duplex
- separate TX/RX per direction

---

## 13. Extensibility

The protocol supports extension via:

- new packet types
- capability flags
- versioning in header

Backward compatibility must be considered.

---

## 14. Future Work

- formal state machines
- security model
- error classification
- advanced routing
- bandwidth optimization
- synchronization features

---

## 15. Summary

OpenRailNet provides:

- deterministic train-wide communication
- automatic topology discovery
- scalable architecture
- hardware-agnostic protocol

It is designed to be simple enough for embedded systems while remaining extensible for future use.