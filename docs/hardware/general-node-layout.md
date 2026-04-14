# General Node Layout

Each OpenRailNet node contains:

## External Interfaces

### Port A
Connection to one neighbor.

### Port B
Connection to the other neighbor.

Both ports are:
- full-duplex communication links
- logically symmetric

## Internal Interfaces

- local bus (SPI, UART, I2C, etc.)
- control outputs (motors, lights)
- sensor inputs

## Responsibilities

- forward packets between A and B
- detect neighbors
- participate in discovery
- execute local actions