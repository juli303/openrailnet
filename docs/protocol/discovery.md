# Discovery

OpenRailNet uses a deterministic discovery process:

1. A node triggers discovery
2. The trigger propagates to a train end
3. The end node starts authoritative discovery
4. The discovery packet traverses the train
5. Each node appends its data
6. The far end generates the consist table
7. The result is broadcast back

This ensures:
- ordered vehicle list
- correct orientation detection
- single authoritative result