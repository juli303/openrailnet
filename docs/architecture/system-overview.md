# System Overview

OpenRailNet forms a linear network of nodes:

Node A ↔ Node B ↔ Node C ↔ ...

Each node:
- has two external links (Port A and Port B)
- forwards packets between them
- exposes local functionality

The system supports:
- arbitrary train length
- mixed vehicle types
- dynamic topology changes