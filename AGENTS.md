# MercyHF Unreal Project Rules & Architecture for AI Agents (Jules, Claude, etc.)

Welcome to the **Mercy Maze (MercyHF)** project! This is a high-quality cooperative Unreal Engine 5 horror game project built with C++ and Blueprint integrations.

To make development stable, efficient, and free of bugs, follow these rules and project structures.

---

## 🗺️ Project Level Loop & Architecture

The game follows a seamless cooperative gameplay loop across three main levels and a main menu:

```
[Index0_Game] (Prologue)
      │
      ▼ (via AMercyLevelTransition)
[Index1_Game] (Abyss & Airlock)
      │
      ▼ (via AMercyLevelTransition)
[Index2_Game] (INDEX-5 Trial Chambers)
      │
      ▼ (via AMercyLevelTransition)
[Main_Menu_Map] (Loop Complete)
```

### 1. `Index0_Game` (The Prologue)
*   **Purpose:** Initial atmospheric entry and prologue tutorial.
*   **Controller:** `AIndex0EntryController` (C++) / `BP_Index0EntryController` (Blueprint) manages the starting blackout sequence, warning text reveals, sound events, and path activation.
*   **Critical Tags:** 
    *   `ENTRY_MAIN_LIGHT` (Main lighting to turn off during blackout)
    *   `ENTRY_RED_LIGHT` (Emergency light to intensify)
    *   `ENTRY_WARNING_TEXT` and `SYSTEM_TEXT` (Warning text signboards)
    *   `ENTRY_PATH_REVEAL` (Transition trigger volume, kept hidden initially)

### 2. `Index1_Game` (Catwalks, Abyss & Airlock)
*   **Purpose:** First major trial zone involving high-altitude catwalks and the bottomless "Abyss".
*   **Features:**
    *   `BP_Checkpoint_Airlock` (`AMercyCheckpointActor`) at X=3850, Y=-950, Z=50 before the Abyss to prevent unfair restarts.
    *   Transition chamber at the end of the catwalks (X=7675, Y=-950, Z=50) containing `BP_LevelTransition_ToIndex2` (`AMercyLevelTransition`) pointing to `Index2_Game`.

### 3. `Index2_Game` (INDEX-5 Chambers of Trials)
*   **Purpose:** Brand new, massive facility comprising a starting Cargo Lift, Decon scanner, Central Observation Hub, and 5 separate branch trial rooms (INDEX-5):
    *   **FEAR (Chamber 1):** Darkness and flashing red light. Contains `BP_Fear_Jumpscare_Trigger` (`AMercyScareEvent`) flashing `BP_Fear_Jumpscare_Monster` for 1.3 seconds, and a `MercyFearZone`.
    *   **GUILT (Chamber 2):** Swamp-green cell block with tables and signboards warning players.
    *   **MEMORY (Chamber 3):** Eerie glossy neon-blue grid with repeating structural columns.
    *   **RESISTANCE (Chamber 4):** Heavy steam pipes and double co-op pressure plates (`I5_RES_PLATE_1`/`2`) connected to `BP_Resistance_CoopController` (`AMercyCoopPuzzleController`). Players must stand on both plates simultaneously to unlock `BP_Resistance_Gate`. Stalked by patrolling AI monster `BP_Resistance_PatrolMonster` using NavMesh.
    *   **MERCY (Chamber 5):** Sterile white chamber ending with the Sewage Outflow escape tunnel.
*   **Escape Route:** The Sewage tunnel ends with an exit gate leading to `BP_LevelTransition_ToEnd` (`AMercyLevelTransition`) pointing to `Main_Menu_Map`, completing the co-op loop.

---

## 🛠️ C++ Classes Reference

Always leverage existing modular C++ classes when creating level logic or triggers:
*   `AMercyLevelTransition` (TargetLevelName, TransitionDelay, bEnabled) - Seamless level loader.
*   `AMercyCheckpointActor` (CheckpointID, SpawnLocationOffset) - Player respawn points.
*   `AMercyCoopPuzzleController` - Activates/deactivates actors based on multi-player overlap.
*   `AMercyScareEvent` - Plays audio-visual jumpscares and handles monster visibility timers.
*   `AMercySystemTextActor` - Modern 3D text renderer with typewriter formatting.

---

## ⚠️ CRITICAL GOTCHAS (READ THIS BEFORE CODING!)

To prevent breaking level geometry, collisions, or aesthetics, adhere strictly to these engineering constraints:

### 📐 1. The Swapped unreal.Rotator Constructor Bug (VERY IMPORTANT!)
*   In the standard Unreal C++ API, `FRotator` is defined as `FRotator(Pitch, Yaw, Roll)`.
*   **HOWEVER**, in the **Unreal Engine Python API**, the constructor signature is **`unreal.Rotator(roll=0.0, pitch=0.0, yaw=0.0)`**!
*   **The Trap:** Writing `unreal.Rotator(0, 90, 0)` under the assumption it represents `(pitch, yaw, roll)` will set **`pitch = 90.0`** and leave `yaw = 0.0`. This makes doors, signboards, or texts lie completely flat on the floor, clip through walls, and block player movement!
*   **The Fix:** 
    *   To set a Yaw of 90 degrees, always write **`unreal.Rotator(0, 0, 90)`** or use keyword arguments: **`unreal.Rotator(yaw=90)`**.
    *   To set a Yaw of 180 degrees, write **`unreal.Rotator(0, 0, 180)`** or **`unreal.Rotator(yaw=180)`**.

### 📝 2. Signboard and Text Alignment Rules
*   3D text actors (`AMercySystemTextActor`) and their backing dark signboard static meshes (`StaticMeshActor`) must be aligned perfectly.
*   **Pivot Rule:** Signboards should share the exact Z-coordinate as their text actor (`bz = t_loc.z`) and the exact rotation (`unreal.Rotator(0, 0, text_yaw)`).
*   **Never** offset the signboard Z-level by +100 units; doing so causes the signboard to float 1 meter above the text, ruining readability.

### 🚫 3. Code & Asset Integrity
*   Do not delete or rename pre-existing assets, maps, blueprints, or plugins.
*   Never modify files inside `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`, or `.vs`.
*   Keep C++ changes clean, modular, and fully compileable. Verify compilation before saving or committing.
