# Packet Format

Each ORN packet consists of:

- Preamble (2 bytes)
- Header
- Payload (variable length)
- CRC16

## Header Fields

- Version + Flags
- Packet Type
- Priority
- Source ID
- Destination ID
- Sequence Number
- Payload Length

## Design Goals

- easy parsing
- minimal overhead
- robust error detection