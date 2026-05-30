# MercyHF Level Plan

This document is the build-out plan for the playable vertical slice. Each INDEX section describes intent, the actors to place, the tags to set, and which C++ controller drives the moment. Refer to `MercyHF_Systems.md` for what each class does in detail.

The progression is linear: INDEX-0 awakens the subject, each subsequent INDEX increases pressure, and INDEX-5 reveals the system's recognition. Levels can ship as separate maps or as streamed sublevels.

---

## INDEX-0 — Entry / Awakening

**Map:** `Index0_Game`

**Intent:** The player wakes inside MERCY. Lights are on briefly, then snap to red. Warning text appears. A "help" voice plays in the dark. Eventually a path forward is revealed under the false promise of "VOLUNTARY ENTRY CONFIRMED."

**Controller:** `AIndex0EntryController`. The level Blueprint placed in `Index0_Game` must inherit from this C++ class.

**Required actors:**

| Actor type | Tag | Notes |
|------------|-----|-------|
| Main lights | `ENTRY_MAIN_LIGHT` | Bright white light sources. Switch off during blackout, but keep physical panels visible. Do not move `PointLight_1`. |
| Red lights | `ENTRY_RED_LIGHT` | Idle 80, blackout 450. Do not move `L_Entry_Red`. |
| Warning text | `ENTRY_WARNING_TEXT` | `TextRenderComponent`. Do not move `TXT_DoNotFollowVoices`. |
| Path reveal meshes | `ENTRY_PATH_REVEAL` | Hidden until 8s mark |
| `PlayerStart` | (built-in) | Do not move |

**Audio:** `HelpVoiceSound` falls back to `/Game/MercyMaze/Audio/SFX_Help.SFX_Help` if not assigned.

**Designer checklist:**

- The controller actor exists in the level and inherits from `AIndex0EntryController`.
- The four required tags exist on the right actors.
- All `UPROPERTY` defaults look right (delays, intensities) — adjust on the actor instance, not in C++.
- PIE shows the on-screen debug messages noting how many actors were found per tag.

---

## INDEX-1 — Mercy Cell

**Map name (suggested):** `Index1_MercyCell`

**Intent:** The player is sealed in a small observation cell. The system runs a registration sequence — eight escalating system messages typewritten on a wall display. After the sequence, an "exit" opens that is in fact the path to the next room.

**Controller:** `AIndex1MercyCellController`.

**Layout:**

- Tight cylindrical or hex room (~5x5m).
- One `AMercySystemTextActor` mounted on the far wall, tagged `SYSTEM_TEXT`.
- One observation light overhead, tagged `INDEX1_OBSERVATION_LIGHT`.
- Exit doorway hidden by mesh / collision; tag every blocking actor and the door itself with `INDEX1_EXIT`.

**Tags required:**

- `SYSTEM_TEXT` on the message display.
- `INDEX1_OBSERVATION_LIGHT` on the overhead light.
- `INDEX1_EXIT` on the door / blocking mesh / hidden lighting.

**Audio:**

- `RegistrationSound` — short pulse at sequence start.
- `ExitRevealSound` — ominous tone at exit reveal.

**Designer checklist:**

- `bAutoStart = true` so the sequence kicks off after `InitialDelay` (default 2s).
- Confirm message timing: 8 messages × 4s = 32s before the exit reveals.
- The exit blocking actor is **hidden and uncollidable** at start; the controller flips both flags on reveal.

---

## INDEX-2 — Help Room

**Map name (suggested):** `Index2_HelpRoom`

**Intent:** The room presents itself as rescue. A friendly help-voice plays. The moment the player crosses the trap trigger, doors close and the lights swap from white to red. After staged messaging, a silent unmarked exit appears in a corner.

**Controller:** `AIndex2HelpRoomController`.

**Layout:**

- Mid-size square room (~10x10m).
- Two doors on opposite walls — both `AMercyDoorController` actors tagged `HELP_ROOM_DOOR`.
- Main lighting tagged `HELP_ROOM_MAIN_LIGHT`. Red emergency lighting tagged `HELP_ROOM_RED_LIGHT`.
- A hidden side exit (mesh + collision blocker) tagged `HELP_ROOM_SILENT_EXIT`.
- A trap trigger box across the entrance walkway.

**Sequence:**

1. Player overlaps `TrapTrigger` → `StartHelpTrap`.
2. `FirstMessageDelay` (1s): "WE WILL HELP YOU".
3. `SecondMessageDelay` (4s): "DO NOT MOVE".
4. Doors close (`HELP_ROOM_DOOR` controllers). Main lights off, red lights on. `TrapCloseSound` plays.
5. `ExitRevealDelay` (12s): silent exit revealed.

**Designer checklist:**

- `HELP_ROOM_DOOR` actors are `AMercyDoorController` Blueprints that respond to `CloseDoor`.
- The silent exit is hidden and uncollidable at start.
- `HelpVoiceSound` is set to a calm, fake-friendly clip; `TrapCloseSound` is a heavy mechanical lock.

---

## INDEX-3 — Moving Walls

**Map name (suggested):** `Index3_MovingWalls`

**Intent:** Spatial horror. The maze rearranges itself when the player isn't looking. Red point lights mark dead ends. System text taunts the player from the walls.

**Controller:** `AIndex3WallsGenerator`.

**Layout:**

- One `AIndex3WallsGenerator` actor at the desired maze origin.
- Set `LayoutRows` to a list of equal-length strings. Default symbols:
  - `#` — wall
  - `.` — floor only
  - `R` — floor + red point light
  - `T` — floor + system text actor
- `bGenerateOnBeginPlay` is true by default. Toggle off if you want to call `GenerateWallsIndex` manually from a sequence.

**Suggested layout (15x15 grid example):**

```
###############
#.............#
#.###.###.###.#
#.#.....R...#.#
#.#.#####.#.#.#
#.#.#...#.#.#.#
#.#.#.#.#.#.#.#
#.#...#T#...#.#
#.#####.#####.#
#.............#
###############
```

**Designer checklist:**

- Assign `CubeMesh` (default cube), `WallMaterial`, `FloorMaterial`, `CeilingMaterial`.
- Set `RandomSeed` so layout is deterministic between PIE sessions.
- `bClearPreviousRuntimeGeneration = true` keeps regeneration clean.

**Future work:** Add a `RegenerateOnPlayerOutOfSight` mode and a "drift" mode where individual walls slide.

---

## INDEX-4 — First Death

**Map name (suggested):** `Index4_FirstDeath`

**Intent:** A corridor with a fake exit. As the player approaches the exit, the corridor closes, a monster reveals behind them, the chase plays for a few seconds, the player is "terminated", then respawned at the start of INDEX-4.

**Controller:** `AIndex4FirstDeathController`.

**Layout:**

- Long corridor (~20m) ending in a fake exit door.
- A box trigger across the end of the corridor.
- Monster mesh / Blueprint placed off-screen, tagged `INDEX4_MONSTER`. Hidden at start.
- The fake exit (door / portal) tagged `INDEX4_FAKE_EXIT`.

**Sequence:**

1. Player enters trigger → `StartFirstDeath(VictimActor)`.
2. `ChaseStartDelay` (2s): reveal `INDEX4_MONSTER`, play `ChaseSound`.
3. `TerminationDelay` (6s): hide `INDEX4_FAKE_EXIT`, play `DeathSound`.
4. `RespawnDelay` (3s): restore player at the level checkpoint.

**Designer checklist:**

- Fake exit is visible at start, hidden on death.
- Monster is hidden at start, revealed on chase.
- A `AMercyCheckpointActor` is placed at the level entrance.
- A `AMercyRespawnComponent` is on the player pawn.

---

## INDEX-5 — Personal Index

**Map name (suggested):** `Index5_PersonalIndex`

**Intent:** The system shifts from third-person ("THE SUBJECT") to second-person ("YOU"). Personal data the system supposedly knows about the player is recited. Each chamber is themed to a category of recognition (name, fear, history, choice).

**Controller:** `AIndex5PersonalIndexController`.

**Layout:**

- A linear arrangement of small chambers, each separated by a door (`AMercyDoorController`).
- One `AMercySystemTextActor` per chamber, tagged `SYSTEM_TEXT` and a chamber-specific tag.
- Per-chamber actors to reveal / hide tagged so the controller's `FMercyPersonalIndexChamber` entry can address them.

**Chamber array (designer-defined):**

Each entry of `Chambers` has `ChamberId`, `ChamberTitle`, `SystemMessage`, `ActorsToShowTag`, `ActorsToHideTag`, and `ChamberDuration`. Suggested progression:

| ChamberId | Title | Sample message | Duration |
|-----------|-------|----------------|----------|
| RECOGNITION | "THE INDEX SEES YOU" | "WE KNOW WHO YOU ARE" | 7s |
| HISTORY | "RECORDED" | "WE REMEMBER WHAT YOU DID" | 7s |
| FEAR | "MEASURED" | "FEAR LEVEL: ELEVATED" | 7s |
| CHOICE | "OFFERED" | "YOU MAY STAY" | 9s |

**Designer checklist:**

- `bAutoStart = false`. Trigger via a `AMercyTriggerVolume` at the chamber entrance.
- Each chamber's `ActorsToShowTag` / `ActorsToHideTag` is unique.
- `ChamberAdvanceSound` and `CompletionSound` are set.

---

## Cross-Level Systems

These are placed once per level (or once per project) regardless of INDEX:

- **Player pawn** with `UMercyProtocolComponent` and `UMercyRespawnComponent`.
- **GameMode** using `AMercyGameState` and `AMercyPlayerState`.
- **At least one `AMercyCheckpointActor`** per level.
- **Level transition trigger** (`AMercyLevelTransition`) at the level exit, pointing at the next map.

---

## Open Build Items (priority order)

1. **INDEX-0 — Verify in PIE.** Open `Index0_Game`, confirm controller Blueprint inherits from `AIndex0EntryController`, confirm tag counts in the on-screen debug. (No code change needed; verification only.)
2. **Create level maps for INDEX-1..INDEX-5.** Each map needs a controller actor, the tagged scene actors listed above, and a `PlayerStart`.
3. **Wire `AMercyLevelTransition`** between levels so the slice plays continuously.
4. **Bake placeholder audio** for `RegistrationSound`, `ExitRevealSound`, `HelpVoiceSound`, `TrapCloseSound`, `ChaseSound`, `DeathSound`, `ChamberAdvanceSound`, `CompletionSound`. Real audio later.
5. **Save/load smoke test.** Confirm `UMercySaveGame` round-trips through INDEX-1 → INDEX-2.
