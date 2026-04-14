# Packet Format

This document defines the OpenRailNet (ORN) packet format and all field-level semantics.

---

## Frame Structure

Each packet consists of:

- Preamble (2 bytes)
- Header
- Payload (variable length)
- CRC16 (2 bytes)

---

## Preamble

| Field | Value |
|------|------|
| PRE1 | 0x55 |
| PRE2 | 0xD5 |

Used for frame synchronization.

---

## Header Fields

| Field     | Size | Description |
|----------|------|-------------|
| VERFLG   | 1    | Version + flags |
| TYPE     | 1    | Packet type |
| PRIORITY | 1    | Priority level |
| SRC      | 2    | Source node ID |
| DST      | 2    | Destination node ID |
| SEQ      | 1    | Sequence number |
| LEN      | 1    | Payload length |

---

## Version + Flags

Upper 4 bits: protocol version  
Lower 4 bits: flags

### Flags

| Bit | Meaning |
|-----|--------|
| 0 | ACK requested |
| 1 | ACK packet |
| 2 | Forwarded packet |
| 3 | Reserved |

---

## Priority Levels

| Value | Meaning |
|------|--------|
| 0 | Emergency |
| 1 | Control |
| 2 | Normal |
| 3 | Diagnostic |

---

## Addressing

- `SRC` = sender node ID
- `DST` = receiver node ID

Special values:

- `0xFFFF` → broadcast

---

## Sequence Number

- 8-bit counter (wrap-around)
- Incremented for each new logical packet
- Retransmissions must reuse the same SEQ

Used for:
- ACK matching
- duplicate detection
- retransmission control

---

## CRC16

### Definition

- Algorithm: **CRC-16/CCITT-FALSE**
- Polynomial: `0x1021`
- Initial value: `0xFFFF`
- Final XOR: `0x0000`
- Input reflected: No
- Output reflected: No

### Coverage

CRC is calculated over:

- Header (starting from VERFLG)
- Payload

Excluded:
- Preamble

### Purpose

- detect transmission errors
- prevent invalid packet execution

---

## Payload Rules

- Maximum length: 255 bytes
- Structure depends on packet type
- Variable-length payloads must define:
  - base struct
  - repeated elements (if applicable)

---

## Capability Flags

Capabilities are defined as a 16-bit bitfield.

### Capability Bits

| Bit | Meaning |
|-----|--------|
| 0 | Supports traction control |
| 1 | Supports braking control |
| 2 | Supports lighting control |
| 3 | Provides sensor data |
| 4 | Supports diagnostics |
| 5 | Supports discovery participation |
| 6 | Reserved |
| 7 | Reserved |
| 8–15 | Future use |

---

## Design Notes

- Packet format is optimized for small MCUs
- Header is fixed size for simple parsing
- Payload is flexible for extensibility
- CRC ensures robustness on noisy links