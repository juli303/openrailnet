# OpenRailNet (ORN)

OpenRailNet (ORN) is an open protocol and system architecture for communication between modular rail vehicles.

It is designed to enable:
- automatic train composition discovery
- multi-locomotive coordination
- distributed control of vehicle subsystems
- scalable and interoperable hardware implementations

OpenRailNet is not just a protocol. It is a complete **open ecosystem** consisting of:
- a communication protocol
- a system architecture
- a hardware interface model
- reference implementations (firmware + hardware)

---

## 🧠 What this project is

OpenRailNet defines **how rail vehicles communicate with each other**.

Each vehicle (locomotive, wagon, etc.) contains a node that:
- connects to neighboring vehicles via two links (Port A and Port B)
- participates in a train-wide network
- exposes local functionality (e.g. traction, lights, sensors)

The system allows vehicles to:
- detect neighbors automatically
- determine their position in the train (consist)
- coordinate behavior across the entire train

---

## 🎯 Goals

- Open and extensible protocol
- Hardware-agnostic communication layer
- Deterministic and debuggable behavior
- Simple implementation on embedded systems (RP2040, ESP32, etc.)
- Support for real-time control (e.g. traction)
- Automatic topology discovery (no manual configuration)

---

## 📦 Scope

This repository includes:

### 1. Protocol Definition
- Packet structure
- Addressing
- Discovery and topology management
- Keepalive and fault detection
- Control and status messaging

### 2. System Architecture
- Node roles (loco, wagon, gateway)
- Network topology (A/B link chain)
- Consist management
- State machines

### 3. Hardware Architecture
- General node layout (Port A / Port B / Local interface)
- Link interface definitions (e.g. RS-485 full duplex)
- Power and signal considerations
- Coupler signal concepts

### 4. Reference Implementations (work in progress)
- Portable protocol library (C)
- PlatformIO firmware targets (RP2040, ESP32)
- Hardware designs (schematics, PCB, BOM)

---

## 🏗️ Project Structure

```text
openrailnet/
├─ docs/          → Human-readable specification
├─ protocol/      → Portable reference implementation (C)
├─ hardware/      → Hardware architecture + implementations
├─ firmware/      → Platform-specific implementations (PlatformIO)
├─ tools/         → Debugging and simulation tools
├─ examples/      → Minimal examples and walkthroughs