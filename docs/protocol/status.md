# Status and Control Definitions

This document defines status fields, control semantics, and error codes.

---

## Direction Semantics

### Consist Direction

Defined by `TRAIN_COMMAND.direction`:

| Value | Meaning |
|------|--------|
| 0 | Hold |
| 1 | Forward |
| 2 | Reverse |

This is always relative to the **consist**, not the vehicle.

---

## Loco Direction Interpretation

Each loco must translate:

- consist direction
- its own orientation

into:

- local motor direction

### Example

If:
- consist direction = forward
- loco orientation = reversed

Then:
- local motor direction = reverse

---

## LOCO_STATUS.actual_direction

This field represents:

> **local physical direction of the vehicle**

Not consist direction.

---

## Status Flags

Defined as 8-bit bitfield.

### Generic Status Flags

| Bit | Meaning |
|-----|--------|
| 0 | Operational |
| 1 | Fault present |
| 2 | Overtemperature |
| 3 | Undervoltage |
| 4 | Communication degraded |
| 5 | Reserved |
| 6 | Reserved |
| 7 | Reserved |

---

## Error Flags

Defined as 16-bit bitfield.

### Error Codes

| Bit | Meaning |
|-----|--------|
| 0 | Motor fault |
| 1 | Brake fault |
| 2 | Sensor failure |
| 3 | Communication timeout |
| 4 | Power fault |
| 5–15 | Reserved |

---

## Emergency Stop

### Behavior

- Must be broadcast
- Must use highest priority
- Must be executed immediately
- Must override all other commands

---

### Reason Codes

| Value | Meaning |
|------|--------|
| 0 | Unknown |
| 1 | User triggered |
| 2 | Safety system |
| 3 | Communication failure |
| 4 | Hardware fault |
| 5 | Software fault |
| 6–255 | Reserved |

---

## ACK Behavior

### When to request ACK

ACK should be requested for:

- configuration changes
- discovery control
- critical commands

---

### When NOT to request ACK

ACK should NOT be used for:

- continuous control (e.g. speed updates)
- periodic status messages

---

### Retry Policy (Recommended)

- timeout: 100–300 ms
- max retries: 3–5
- retransmissions use same SEQ

---

## Design Notes

- Separation of consist vs local direction is critical
- Status flags must remain simple and extensible
- Emergency stop must be deterministic and immediate