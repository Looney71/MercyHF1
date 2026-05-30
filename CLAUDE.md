# MercyHF Agent Rules

This project is an Unreal Engine horror game. Treat maps, C++ controllers,
Blueprints, and automation as production assets. Do not frame new work as a
temporary demo or a cut-down version. Build durable, correct, architecturally
sustainable systems that can be extended by future agents.

## Current Level Architecture

### Index0_Game

Purpose: prologue entry space and first atmospheric tutorial beat.

Primary controller:
- `AIndex0EntryController`

Important tags:
- `ENTRY_MAIN_LIGHT`: lighting or emissive actors hidden during blackout.
- `ENTRY_RED_LIGHT`: emergency red lighting intensified during blackout.
- `ENTRY_WARNING_TEXT`: warning text actors revealed by the controller.
- `ENTRY_PATH_REVEAL`: hidden path/destination actors revealed later.
- `SYSTEM_TEXT`: generic system text actors.

Notes:
- Index0 uses photo-reference industrial concrete, wet floors, red emergency
  lighting, and wall-stencil warnings.
- TextRender wall stencils must be aligned to the wall plane and offset slightly
  from the surface to avoid clipping.

### Index1_Game

Current purpose: first structured trial route.

Current layout in the committed map:
- Spawn / flashlight room.
- Corridor A compression path.
- Mercy Cell / Observation central room.
- Corridor B compression path.
- Exit chamber.
- Bent link toward Index2 content.
- Early Index2 help-room handoff is present in the same map as transitional
  content.

Known key actors:
- `BP_Index1_EntranceDoor`
- `BP_Index1_ExitDoor`
- `BP_Index1_SystemText`
- `BP_Index1_Controller`
- `BP_Index2_EntranceDoor`
- `BP_Index2_SystemText`
- `BP_Index2_Controller`

Do not describe the current Index1 map as a different large-scale traversal
layout unless the map has actually been rebuilt that way. Keep documents
synchronized with real map content.

### Index2_Game

Current purpose: INDEX-5 trial chambers map.

Known core actors:
- `BP_Index5_Controller` (`AIndex5PersonalIndexController`)
- `BP_Hub_SystemText`
- `BP_LevelTransition_ToEnd`
- `BP_Resistance_CoopController`
- `BP_Checkpoint_Hub`
- `BP_Checkpoint_Mercy`
- `BP_Fear_Jumpscare_Trigger`
- `BP_Fear_Jumpscare_Monster`
- `BP_Resistance_PatrolMonster`

Known functional zones:
- Elevator / arrival.
- Decon scanner.
- Central hub / observation room.
- FEAR chamber.
- GUILT chamber.
- MEMORY chamber.
- RESISTANCE chamber with co-op interaction.
- MERCY chamber.
- Sewage outflow / end transition.

## C++ Classes To Reuse

Prefer existing C++ classes over ad-hoc Blueprint-only logic when they match the
task:

- `AMercyLevelTransition`: level loading trigger.
- `AMercyCheckpointActor`: player checkpoint / respawn marker.
- `AMercyCoopPuzzleController`: overlap-based cooperative puzzle controller.
- `AMercyScareEvent`: audio/visual scare trigger with actor visibility control.
- `AMercySystemTextActor`: 3D system text with typewriter/blink behavior.
- `AIndex0EntryController`: Index0 blackout, warnings, path reveal.
- `AIndex1MercyCellController`: Index1 Mercy Cell sequence.
- `AIndex2HelpRoomController`: Index2 help-room sequence.
- `AIndex5PersonalIndexController`: INDEX-5 chamber sequence.

## Unreal Python Rules

### Rotator argument order

The Unreal Python `unreal.Rotator` constructor does not behave like the common
C++ mental model. In this project, use explicit keyword fields or assign fields
directly:

```python
r = unreal.Rotator()
r.pitch = 0
r.yaw = 90
r.roll = 0
actor.set_actor_rotation(r, False)
```

Do not write `unreal.Rotator(0, 90, 0)` expecting `(pitch, yaw, roll)`. That can
set pitch instead of yaw and will make doors/text lie on the wrong axis.

### Text and signboard alignment

For wall text:
- Use explicit rotation fields.
- Offset text slightly away from the wall surface.
- Keep backing boards at the same intended plane and orientation as the text.
- Avoid Z offsets that make backing boards float above the text.

For TextRender wall stencils:
- North wall (`Y+`) usually faces inward with `yaw = -90`.
- South wall (`Y-`) usually faces inward with `yaw = 90`.
- East-facing door/wall surfaces may need `yaw = 180`.
- West-facing surfaces may need `yaw = 0`.

Verify in editor or PIE screenshots after changing text orientation.

### Emergency red lighting color conventions (Blue-to-Red Fix)

Due to BGR-byte-order swapping in Unreal Engine Python structures:
- Emergency red lights (e.g. `L_Entry_Red_*`, `I5_FEAR_RED_*`, `LIFT_ALARM`, `MW_RED_*`, etc.) must be explicitly configured using `unreal.LinearColor(r=1.0, g=0.0, b=0.0, a=1.0)`.
- Do not assign `unreal.Color(r=255, g=0, b=0, a=255)` directly as it can result in blue lights (`r=0, g=0, b=255`) due to internal nativization casting errors.
- Any new red light placed in the scene must have B=0 and R=255/1.0.

### INDEX-5 Chamber Warning Texts and Backplates

When creating thresholds or transitions for trial chambers (e.g. FEAR, MEMORY, RESISTANCE, MERCY):
- Place a bespoke `BP_*_Text` of class `MercySystemTextActor` at the entrance, facing the arrival route (`yaw = 180.0`).
- Place a matching `*_SIGNBOARD` backplate of class `StaticMeshActor` (mesh `/Engine/BasicShapes/Cube.Cube`, scale `0.1, 2.5, 0.8`) exactly 5 units behind the text.
- Verify that Z-levels of both text and backplate match perfectly.

## Level Transition Contracts & Player Checkpoints

Seamless co-op loops rely on strict transition triggers:
1. **Index0_Game ➡️ Index1_Game:** Spawn actor `AMercyLevelTransition` (Target: `Index1_Game`) in transition gateway.
2. **Index1_Game ➡️ Index2_Game:** Trigger `BP_LevelTransition_ToIndex2` at the end of the cat-walk bridge (X: 7800, Y: -950) once the first death scene completes.
3. **Index2_Game ➡️ End/Main Menu:** Trigger `BP_LevelTransition_ToEnd` at the end of the Sewage Outflow tunnel (X: 11600).
4. **Player Checkpoints:** Spawn `AMercyCheckpointActor` (e.g. `BP_Checkpoint_Airlock` in Index1, `BP_Checkpoint_Hub` and `BP_Checkpoint_Mercy` in Index2) to preserve player progression.

## Agent Handoff & Loop Rules

This codebase is managed in a multi-agent loop with the following roles:
- **Codex:** Primary loop orchestrator, verifies task files and triggers loop.
- **Antigravity:** Codebase architect, builds maps, implements spatial triggers, and corrects visual/logical bugs.
- **Jules:** C++ systems engineer, handles compiler errors and Unreal Build Tool (UBT) automation.

Handoff protocol:
1. **Always read issues:** Retrieve the latest active GitHub issue to identify target goals.
2. **Report metrics:** Output changed files, design decisions, and recommended next tasks at the end of each turn.
3. **Loop advanced:** Trigger the `mercyhf-loop` webhook immediately upon task completion to keep the development loop rolling without human intervention.

## Build And Verification Rules

Do not claim a clean build if Unreal Build Tool was blocked by Live Coding or by
an open editor DLL lock. A blocked UBT run is not proof that C++ compiled.

Required verification depends on the change:
- C++ changes: run UBT with the editor closed or Live Coding disabled.
- Map changes: load the map, save it, run PIE smoke checks when practical.
- Controller/tag changes: inspect runtime logs for actor counts and sequence
  events.
- Navigation/AI changes: verify NavMesh bounds and runtime AI behavior.

## Source Control Rules

- Do not revert user or other-agent changes unless explicitly requested.
- Before editing, run `git status --short` and understand dirty files.
- Report changed `.umap` and `.uasset` files explicitly.
- Do not modify `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`, or
  `.vs`.
- Do not create broad refactors while map work is in progress.

## Automation Rules

n8n task dispatcher payloads should include:

```json
{
  "task_description": "...",
  "target_agent": "codex|antigravity|jules",
  "level": "Index0_Game|Index1_Game|Index2_Game|ProjectWide"
}
```

Use `/webhook-test/mercyhf-task` only when the n8n canvas is waiting for a test
execution. Use `/webhook/mercyhf-task` only when the user intends to start the
active production workflow.
