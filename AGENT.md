# AGENT.md

## Project

**OpenRailNet (ORN)** is an open protocol and system architecture for communication between modular rail vehicles.

The project is intended to grow into an open ecosystem consisting of:
- a protocol specification
- a portable reference library
- hardware architecture documentation
- concrete hardware implementations
- firmware implementations

This file is a handover and working guide for agents and contributors so future work stays aligned with the intended architecture.

---

## Primary Goal

OpenRailNet defines **how rail vehicles communicate with each other** in a modular consist.

Each vehicle contains a node that:
- connects to neighbors via **Port A** and **Port B**
- participates in a train-wide network
- may expose local functions such as traction, lights, sensors, and diagnostics

Key intended capabilities:
- automatic train composition discovery
- multi-locomotive coordination
- distributed subsystem control
- deterministic and debuggable communication
- reusable protocol across different MCU platforms

---

## Core Architectural Layers

The project should be treated as four distinct layers:

### 1. Protocol Specification
Defines:
- packet structure
- addressing
- discovery behavior
- keepalive/liveness behavior
- status/control semantics
- reliability rules

This is the normative definition of system behavior.

Main locations:
- `docs/protocol/`
- `docs/architecture/`

---

### 2. Portable Protocol Library
Defines:
- C structs for wire format
- CRC
- encode/decode helpers
- discovery helpers
- sequence/ACK helpers
- protocol logic that is platform-independent

This must remain portable and independent of PlatformIO, Arduino, Pico SDK, ESP-IDF, or any board-specific code.

Main location:
- `protocol/`

---

### 3. Platform Integration Layer
Defines:
- UART / RS-485 handling
- timers
- scheduling
- pin mappings
- MCU/board integration

This is where PlatformIO belongs.

Main location:
- `firmware/platformio/`

---

### 4. Concrete Implementations
Defines:
- actual node devices
- real hardware designs
- firmware targets for specific boards and node roles

Examples:
- RP2040 gateway node
- RP2040 loco node
- ESP32 node

Main locations:
- `hardware/implementations/`
- `firmware/platformio/`

---

## Source of Truth Rules

These rules are extremely important.

### Normative hierarchy
1. `docs/protocol/spec.md`
2. `protocol/include/*.h`
3. `docs/protocol/*.md`
4. `docs/hardware/*.md` and `hardware/reference/`
5. `hardware/implementations/` and `firmware/`

### Interpretation
- The **protocol spec** defines intended behavior.
- The **protocol headers** define the current wire-level implementation.
- The explanatory docs should be aligned to those two.
- Concrete implementations must **not silently redefine the standard**.

### Rule
If implementation code differs from the normative protocol spec:
- do **not** silently rewrite the spec to match code
- instead, flag the conflict explicitly

---

## Current Protocol Decisions

These points were decided in this design phase and should be preserved unless intentionally revised.

### Network Model
- Linear chain of nodes
- Each node has:
  - `Port A`
  - `Port B`
  - local internal interfaces
- Full-duplex point-to-point links between neighboring vehicles
- Store-and-forward behavior inside each node

### Physical Layer Direction
Current reference direction is:
- **full-duplex RS-485**
- separate TX/RX paths
- no shared-bus arbitration required

### Discovery Model
Discovery is deterministic.

Flow:
1. A node sends `DISCOVER_TRIGGER` in one direction only
2. The trigger propagates until reaching an end node
3. The end node starts `DISCOVER_AUTH`
4. The discovery packet traverses the full consist
5. Each node appends its record
6. The far end builds the final topology
7. The far end sends back `CONSIST_TABLE`

Important:
- only end nodes generate authoritative discovery
- no merging of two half-lists
- no fixed global master required

### End Node Detection
Protocol-only approach.

A node continuously checks neighbor liveness on both ports.

A node is considered an end if:

`alive_A XOR alive_B == true`

### Liveness / Coupling
Neighbor checking is continuous, not startup-only.

This is intentional so the system can:
- detect coupling
- detect uncoupling
- detect dead neighbors
- trigger re-discovery automatically

### Direction Semantics
Two distinct concepts must remain separate:

- **consist-relative direction**
- **local physical direction of a vehicle**

`TRAIN_COMMAND.direction` is always in **consist coordinates**.

`LOCO_STATUS.actual_direction` is currently defined as:
- the **local physical direction of the vehicle**
- not consist-relative direction

### Orientation
Each node stores which physical side is its front:
- `ORN_FRONT_A`
- `ORN_FRONT_B`

This is later resolved into:
- `ORN_ORIENT_NORMAL`
- `ORN_ORIENT_REVERSED`

relative to the consist.

### Reliability
Protocol includes:
- sequence numbers (`SEQ`)
- ACK/NACK support
- duplicate detection using:
  - `SRC + TYPE + SEQ`

Retransmissions must reuse the same sequence number.

### CRC
Defined in the docs as:
- **CRC-16/CCITT-FALSE**
- polynomial `0x1021`
- init `0xFFFF`
- no reflection
- no final XOR

---

## Repository Layout (Intended)

Target structure:

```text
openrailnet/
├─ README.md
├─ AGENT.md
├─ docs/
│  ├─ overview/
│  ├─ protocol/
│  ├─ architecture/
│  └─ hardware/
├─ protocol/
│  ├─ include/
│  ├─ src/
│  ├─ tests/
│  └─ library.json
├─ hardware/
│  ├─ reference/
│  └─ implementations/
├─ firmware/
│  ├─ README.md
│  └─ platformio/
│     ├─ rp2040-gateway-node/
│     ├─ rp2040-loco-node/
│     └─ esp32-node/
├─ tools/
└─ examples/