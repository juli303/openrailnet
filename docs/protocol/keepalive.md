# Keepalive and Link Detection

Each node continuously monitors both ports:

- periodic HELLO messages
- timeout-based liveness detection
- detection of coupling/uncoupling events

Topology changes trigger re-discovery.