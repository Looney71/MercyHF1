# Index1_Game Top-Down Plan

Reference image:

`Docs/LevelPlans/Index1_TopDown_Plan_v2.png`

## Current Route

1. `IDX1_ROUTE_00_SPAWN`
2. `IDX1_ROUTE_01_FLASHLIGHT_PICKUP`
3. `IDX1_ROUTE_02_CORRIDOR_A_START`
4. `IDX1_ROUTE_03_INDEX1_ENTRANCE`
5. `IDX1_ROUTE_04_MERCY_CELL_CENTER`
6. `IDX1_ROUTE_05_INDEX1_EXIT`
7. `IDX1_ROUTE_06_CORRIDOR_B`
8. `IDX1_ROUTE_07_EXIT_CHAMBER`
9. `IDX1_ROUTE_08_BENT_LINK`
10. `IDX1_ROUTE_09_INDEX2_HANDOFF_DOOR`
11. `IDX1_ROUTE_10_HELP_ROOM_CENTER`
12. `IDX1_ROUTE_11_SILENT_EXIT_HINT`

## Event Anchors

- `IDX1_EVT_AUTO_DOOR_CLOSE_BEHIND`
- `IDX1_EVT_OBSERVATION_SCAN_CENTER`
- `IDX1_EVT_MERCY_CELL_RED_ENTRANCE`
- `IDX1_EVT_MERCY_CELL_RED_EXIT`
- `IDX1_EVT_EXIT_CHAMBER_PRESSURE`
- `IDX1_EVT_INDEX2_HELP_ROOM_RED_STATE`

## Tags

- `INDEX1_ARCHITECTURE`
- `INDEX1_ZONE_ANCHORS_V1`
- `INDEX1_ROUTE_ANCHOR`
- `INDEX1_EVENT_ANCHOR`
- `INDEX1_ZONE_SPAWN`
- `INDEX1_ZONE_CORRIDOR_A`
- `INDEX1_ZONE_MERCY_CELL`
- `INDEX1_ZONE_CORRIDOR_B`
- `INDEX1_ZONE_EXIT_CHAMBER`
- `INDEX1_ZONE_BENT_LINK`
- `INDEX2_HANDOFF`

## Design Notes

The current structure is intentionally readable:

- safe start and flashlight acquisition;
- first compression corridor;
- wider watched Mercy Cell / Observation room;
- second compression corridor;
- exit chamber;
- bent link that hides the Index2 handoff until late.

Future work should attach logic, lighting, audio, and checkpoints to these named
anchors instead of hard-coding anonymous coordinates in one-off scripts.
