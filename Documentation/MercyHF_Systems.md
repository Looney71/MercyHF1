# MercyHF Gameplay Systems

This document inventories the C++ gameplay systems shipped under `Source/MercyHF`. It describes what each class is, how it is meant to be wired up in a level (tags, Blueprint subclasses, expected actor names), and which knobs designers can tune from the editor without touching code.

The convention across the project is:

- C++ classes live under `Source/MercyHF/Public` (headers) and `Source/MercyHF/Private` (implementations).
- Designers drop the actor into a level (or a Blueprint subclass of it) and identify scene actors with **Actor Tags**.
- Tags use the `ALL_CAPS_WITH_UNDERSCORES` convention.
- Every controller writes `UE_LOG(LogTemp, Warning, ...)` and on-screen debug messages so behavior is visible during PIE.

---

## INDEX Controllers

These actors orchestrate per-level scripted moments. Place exactly one per level (or one per area).

### `AIndex0EntryController` (Index0EntryController.h)

The awakening sequence. Caches scene actors by tag (or by name fallback) and runs four staged events on timers.

| Stage | Default Delay | Effect |
|-------|---------------|--------|
| Idle  | 0s            | Main lights on, red lights at idle intensity, warning text hidden, path hidden |
| Blackout | 2.0s       | Disable main light components/emissive materials while keeping physical panels visible; raise red lights to blackout intensity |
| Warning text | 2.35s | Set "DO NOT FOLLOW VOICES" and reveal `ENTRY_WARNING_TEXT` actors |
| Help voice | 4.0s    | Play 2D help voice sound (loaded from `HelpSoundAssetPath` if `HelpVoiceSound` empty) |
| Path reveal | 8.0s   | Reveal `ENTRY_PATH_REVEAL` actors and update warning text to "VOLUNTARY ENTRY CONFIRMED" |

Required scene tags:

- `ENTRY_MAIN_LIGHT` — every white/main light in the entry room
- `ENTRY_RED_LIGHT` — red emergency lights (must own a `LightComponent`)
- `ENTRY_WARNING_TEXT` — actors with a `TextRenderComponent`
- `ENTRY_PATH_REVEAL` — meshes that reveal the path forward

Designer notes:

- The Blueprint placed in `Index0_Game` must inherit from `AIndex0EntryController`.
- All delays, intensities, and the help-sound asset path are `UPROPERTY(EditAnywhere)` and tunable on the actor.
- Tag fallback: actor name match works for legacy actors named `L_Entry_Main`, `L_Entry_Red`, `TXT_DoNotFollowVoices`, or containing `Entry_Path`.

### `AIndex1MercyCellController`

Pressure-room sequence. Cycles a list of system messages on `SYSTEM_TEXT` actors using `AMercySystemTextActor::ShowTypewriterMessage`, then reveals `INDEX1_EXIT` actors and brightens `INDEX1_OBSERVATION_LIGHT` lights.

Default message sequence (override via the `MessageSequence` array):

1. PERSONAL INDEX ACTIVE
2. SUBJECT: UNREGISTERED
3. PLEASE REMAIN STILL
4. MOVEMENT REGISTERED
5. COMPLIANCE: LOW
6. REGISTRATION INCOMPLETE
7. ACCESS TO INDEX-2 GRANTED
8. THIS IS NOT AN EXIT

Blueprint-callable: `StartMercyCellSequence`, `RevealExit`.

### `AIndex2HelpRoomController`

False rescue trap. The player overlapping `TrapTrigger` triggers staged messages, swaps `HELP_ROOM_MAIN_LIGHT` to `HELP_ROOM_RED_LIGHT`, closes `HELP_ROOM_DOOR`-tagged door controllers, and finally reveals a `HELP_ROOM_SILENT_EXIT`.

Use Blueprint nodes `StartHelpTrap` / `RevealSilentExit` for scripted overrides.

### `AIndex3WallsGenerator`

Procedural maze generator. Reads `LayoutRows` (an array of equal-length strings), spawns floor / wall / ceiling cube actors at `TileSize` cell pitch, places red point lights on `R` cells, and spawns `AMercySystemTextActor` instances on `T` cells.

- `bClearPreviousRuntimeGeneration` removes previously spawned children before regeneration.
- `bCenterMazeOnActor` recenters the layout around the actor's transform.
- `bGenerateOuterWalls`, `bGenerateCeiling` toggle the bounding shell.

The generator tags every spawned actor so other systems can reference them.

### `AIndex4FirstDeathController`

First-death scripted moment. A box trigger overlap calls `StartFirstDeath(VictimActor)`:

1. Reveal `INDEX4_MONSTER` actors after `ChaseStartDelay` (default 2.0s) and play `ChaseSound`.
2. Hide `INDEX4_FAKE_EXIT` and play `DeathSound` after `TerminationDelay` (default 6.0s).
3. Restore the player after `RespawnDelay` (default 3.0s).

System messages are delivered on `SYSTEM_TEXT`-tagged actors.

### `AIndex5PersonalIndexController`

Recognition / escalation. Iterates an editor-defined `Chambers` array (`FMercyPersonalIndexChamber`). Each chamber has a title, a system message, optional actors to show / hide via tag, and a duration. Use `StartPersonalIndex`, `AdvanceChamber`, `CompletePersonalIndex` from Blueprints to script the flow.

---

## Reusable Building Blocks

These are dropped into levels directly — usually multiple per level.

### `AMercySystemTextActor`

The standard system-message actor. A `USceneComponent` root with one `UTextRenderComponent`. Exposes:

- `ShowInstantMessage(Message, AutoHideAfterSeconds)` — set text immediately.
- `ShowTypewriterMessage(Message, CharacterIntervalSeconds, AutoHideAfterSeconds)` — type the message one character at a time.
- `HideMessage()` — hide and clear timers.
- `SetBlinking(bEnableBlinking, BlinkIntervalSeconds)` — pulse on/off.
- `SetMessageColor(NewColor)`, `SetMessageWorldSize(NewWorldSize)`.

Tag every instance with `SYSTEM_TEXT` (and optionally a more specific tag) so controllers can find it.

### `AMercyTriggerVolume`

Generic box trigger. On player overlap (configurable to other actor types) it:

- Plays an optional sound (2D or attached).
- Shows / hides actors via tag or explicit list (`ActorsToShowTag`, `ActorsToHideTag`, plus list versions).
- Toggles collision via tag or list.
- Broadcasts `OnMercyTriggerActivated` for Blueprint subscribers.

`ActivationDelay` lets designers stagger the effect. `bTriggerOnlyOnce` is on by default.

### `AMercyDoorController`

Door logic. Animates a `TargetDoorActor` (resolved by reference, by `MERCY_DOOR` tag, or by name contains) from `ClosedRotation` to `ClosedRotation + OpenYawOffset` at `DoorOpenSpeed`. Supports lock / unlock, auto-open on overlap, optional collision disable while open.

Blueprint-callable: `OpenDoor`, `CloseDoor`, `ToggleDoor`, `LockDoor`, `UnlockDoor`, `IsDoorOpen`, `IsDoorLocked`.

### `AMercyAudioTrigger`

Sphere trigger that plays a `USoundBase` (loaded by direct reference or `SoundAssetPath`) on player overlap or at BeginPlay. Supports 2D / 3D, volume, pitch, start time, replay handling.

### `AMercyFearZone`

Ambient fear / pressure zone. Designed to apply a sustained effect (e.g., audio, post-process, gameplay tag) while the player is inside. See header for current parameters.

### `AMercyDeathVolume`

Insta-kill / reset volume. Overlapping the volume triggers the project's death / respawn flow.

### `AMercyLevelTransition`

Level-transition trigger. Loads the next level / sublevel when the player overlaps.

### `AMercyCheckpointActor`

Persistent checkpoint marker. Saves player state for the respawn system.

### `AMercyLightFlicker`

Drives a light component with a configurable flicker pattern. Attach to or place near a light.

### `AMercyScareEvent`

One-shot scripted scare (sound + actor reveal + optional camera shake). Fired from a trigger, controller, or sequence.

### `AMercyVoiceSequence`

Plays an ordered list of voice clips with per-clip delays. Used for system / mercy / help voice lines.

### `AMercyCoopPuzzleController`

Scaffolding for two-actor / two-button puzzles. Tracks press state and fires events when all inputs are satisfied.

---

## Player & Game State Components

### `AMercyGameState`

Project `GameState` subclass. Owns global progression flags (current INDEX, completed chambers, escalation level).

### `AMercyPlayerState`

Per-player state. Tracks deaths, fear level, registration state.

### `UMercyProtocolComponent`

Component attached to the player pawn. Owns the death / reset protocol pipeline (trigger death, freeze input, run reset, restore).

### `UMercyRespawnComponent`

Handles teleporting the pawn to the active checkpoint and restoring relevant gameplay state.

### `UMercyObjectiveManager`

Tracks current objective text and exposes Blueprint events when objectives change.

### `UMercySaveGame`

`USaveGame` subclass that persists progression between sessions.

---

## Tag Reference

The project uses tags for level / actor wiring. Keep them consistent so controllers find the right actors.

### Cross-level

- `SYSTEM_TEXT` — any `AMercySystemTextActor` that should receive system messages.
- `MERCY_DOOR` — door actor handled by `AMercyDoorController`.

### INDEX-0

- `ENTRY_MAIN_LIGHT`
- `ENTRY_RED_LIGHT`
- `ENTRY_WARNING_TEXT`
- `ENTRY_PATH_REVEAL`

### INDEX-1

- `INDEX1_EXIT`
- `INDEX1_OBSERVATION_LIGHT`

### INDEX-2

- `HELP_ROOM_DOOR`
- `HELP_ROOM_MAIN_LIGHT`
- `HELP_ROOM_RED_LIGHT`
- `HELP_ROOM_SILENT_EXIT`

### INDEX-4

- `INDEX4_FAKE_EXIT`
- `INDEX4_MONSTER`

---

## Conventions for New Systems

- Inherit from `AActor` unless tighter integration with the player or game mode is required.
- Expose tunable values as `UPROPERTY(EditAnywhere, Category = "Mercy Maze|<Subsystem>")` so designers can adjust without recompiling.
- Identify scene actors via `UPROPERTY(EditAnywhere) FName MyTag = TEXT("MY_TAG")`, then `UGameplayStatics::GetAllActorsWithTag(...)`.
- Provide a `bShowDebugMessages` toggle and a small `DebugMessage` helper that logs to `LogTemp` and `GEngine->AddOnScreenDebugMessage`.
- Mark gameplay entry points `UFUNCTION(BlueprintCallable, Category = "Mercy Maze|...")` so they remain scriptable.
- Avoid hardcoded references to specific actor names. Tags first, name-contains as a defensive fallback.
