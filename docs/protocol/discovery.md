# Discovery

This document defines topology discovery and network formation in OpenRailNet.

---

## Overview

Discovery determines:

- node order (consist)
- node orientation
- full network topology

The process is deterministic and produces a single authoritative result.

---

## Discovery Flow

1. Node sends `DISCOVER_TRIGGER` in one direction
2. Packet propagates until reaching an end node
3. End node starts `DISCOVER_AUTH`
4. Discovery packet traverses the train
5. Each node appends its record
6. Far end receives full list
7. Far end generates `CONSIST_TABLE`
8. Table propagates back through the network

---

## Discovery Session

Each discovery uses a:

- `session_id` (16-bit)

Rules:
- must be unique per discovery run
- nodes must ignore outdated session IDs

---

## Node Record (DISCOVER_AUTH)

Each node appends:

- vehicle_id
- vehicle_type
- front_side
- capabilities

---

## Orientation Calculation

Each node declares:

- which physical side is its **front** (A or B)

From this, the system derives:

- whether the node is aligned with consist direction
- or reversed

Final consist table includes:

- position index
- orientation flag

---

## End Detection

A node is an end node if:

- exactly one port is alive

Condition:
is_end = alive_A XOR alive_B

---

## Topology Change Detection

Triggered when:

- new neighbor appears
- neighbor disappears
- neighbor ID changes

---

## Stabilization Delay

Before starting discovery:

- wait 500–1000 ms after topology change

Purpose:
- avoid repeated discovery during coupling/uncoupling

---

## Discovery Trigger Rules

- Any node may initiate discovery
- Only end nodes generate `DISCOVER_AUTH`
- Non-end nodes forward trigger packets

---

## Consist Table

Final result contains:

- ordered list of nodes
- position index
- orientation

This is the authoritative topology.

---

## Failure Handling

- If discovery is interrupted → discard session
- If multiple sessions detected → keep latest
- If timeout occurs → retry discovery

---

## Design Notes

- No merging of partial lists required
- Single-direction authoritative discovery ensures consistency
- Fully distributed, no fixed master required