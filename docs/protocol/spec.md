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

- unique 16-bit node identifier carried in `SRC` / `DST`
- `vehicle_type`
- `front_side` (A or B)
- link interfaces (Port A and Port B)

Discovery records and consist tables refer to the same identifier as
`vehicle_id`.

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

Payload fields:

| Field | Size | Notes |
|------|------|------|
| `vehicle_type` | 1 | `orn_vehicle_type_t` |
| `front_side` | 1 | `orn_front_side_t` |
| `proto_version` | 1 | Usually `ORN_PROTO_VERSION_V0_1` |
| `hello_seq` | 1 | HELLO-local sequence for link management |
| `capabilities` | 2 | Capability bitfield |

Notes:

- sender identity is carried in `HEADER.SRC`
- HELLO does not duplicate `vehicle_id` in the payload

---

### 6.2 HELLO_ACK

Response to HELLO.

Payload fields:

| Field | Size | Notes |
|------|------|------|
| `acked_hello_seq` | 1 | HELLO sequence being acknowledged |
| `status_flags` | 1 | Optional link / status flags |

`HELLO_ACK` is only for neighbor management. It is distinct from the
generic reliability packets `ACK` and `NACK`.

---

### 6.3 DISCOVER_TRIGGER

Initiates discovery.

Payload fields:

| Field | Size | Notes |
|------|------|------|
| `session_id` | 2 | Unique discovery session identifier |
| `trigger_direction` | 1 | `orn_port_t` from original sender perspective |
| `reserved` | 1 | Reserved, transmit as zero |

Behavior:

- sent in one direction only
- used to locate a train end

---

### 6.4 DISCOVER_AUTH

Authoritative discovery packet.

Base payload fields:

| Field | Size | Notes |
|------|------|------|
| `session_id` | 2 | Current discovery session |
| `record_count` | 1 | Number of appended records |
| `reserved` | 1 | Reserved, transmit as zero |

Discovery records appended after the base payload:

| Field | Size | Notes |
|------|------|------|
| `vehicle_id` | 2 | Same logical identifier as `HEADER.SRC` |
| `vehicle_type` | 1 | `orn_vehicle_type_t` |
| `front_side` | 1 | `orn_front_side_t` |
| `capabilities` | 2 | Capability bitfield |

Behavior:

- initiated only by an end node
- each participating node appends exactly one discovery record
- `record_count` must match the number of appended records

---

### 6.5 CONSIST_TABLE

Final topology result.

Base payload fields:

| Field | Size | Notes |
|------|------|------|
| `session_id` | 2 | Discovery session being published |
| `entry_count` | 1 | Number of appended consist entries |
| `reserved` | 1 | Reserved, transmit as zero |

Consist entries appended after the base payload:

| Field | Size | Notes |
|------|------|------|
| `vehicle_id` | 2 | Same logical identifier as `HEADER.SRC` |
| `position` | 1 | `0..N-1` in consist order |
| `orientation` | 1 | `orn_orientation_t` |

Behavior:

- contains the ordered list of nodes
- `entry_count` must match the number of appended entries

---

### 6.6 TRAIN_COMMAND

Control packet.

Payload fields:

| Field | Size | Notes |
|------|------|------|
| `direction` | 1 | `orn_direction_t` in consist coordinates |
| `speed_cmd` | 1 | `0..255` |
| `brake_cmd` | 1 | `0..255` |
| `flags` | 1 | Implementation-defined command flags |

---

### 6.7 EMERGENCY_STOP

Immediate stop command.

Payload fields:

| Field | Size | Notes |
|------|------|------|
| `reason_code` | 1 | Stop reason |
| `source_class` | 1 | Origin class |

Behavior:

- broadcast
- highest priority

---

### 6.8 STATUS (Vehicle / Loco)

Provides runtime information.

`VEHICLE_STATUS` payload fields:

| Field | Size | Notes |
|------|------|------|
| `state_flags` | 1 | Generic status bits |
| `battery_percent` | 1 | `0..100` recommended |
| `temperature_c` | 1 | Signed Celsius |
| `reserved0` | 1 | Reserved, transmit as zero |
| `error_flags` | 2 | Generic error bitfield |

`LOCO_STATUS` payload fields:

| Field | Size | Notes |
|------|------|------|
| `actual_direction` | 1 | Local physical direction, not consist-relative |
| `actual_speed` | 1 | Current speed |
| `traction_percent` | 1 | `0..100` recommended |
| `brake_percent` | 1 | `0..100` recommended |
| `error_flags` | 2 | Loco error bitfield |

---

### 6.9 ACK / NACK

Used for reliable communication.

Packet types:

- `ACK` uses packet type `ORN_PKT_ACK`
- `NACK` uses packet type `ORN_PKT_NACK`

Shared payload fields:

| Field | Size | Notes |
|------|------|------|
| `acked_type` | 1 | Original packet type being acknowledged |
| `acked_seq` | 1 | Original packet sequence number |
| `code` | 1 | `0` for ACK, nonzero error/status for NACK |
| `reserved` | 1 | Reserved, transmit as zero |

ACK flag semantics:

- `VERFLG.bit0` (`ACK requested`) may be set on non-ACK/NACK packets that require a reliability response
- `VERFLG.bit1` (`ACK packet`) is set on both `ACK` and `NACK` frames
- `ACK` and `NACK` do not request acknowledgments of their own

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

Packets may request acknowledgment by setting `VERFLG.bit0`.

- the receiver responds with `ACK` or `NACK` addressed back to the original sender
- the response references the original packet using `acked_type` and `acked_seq`
- sender retransmits the original packet if no response is received
- retransmissions reuse the same `SEQ`
- duplicate packets must not be executed twice, but they should still be answered if acknowledgment was requested

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
