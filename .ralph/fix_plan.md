# MercyHF Full Game Autonomous Sprint

Mission:
Make MercyHF into a playable psychological sci-fi horror vertical slice.

Main instruction:
Work autonomously. Fix broken systems, create missing systems, improve C++, prepare levels, use Unreal MCP when possible, build after code changes, and report everything.

Immediate priority:
Build-verify the `Index2HelpRoomController` migration that just landed, then continue migrating remaining INDEX controllers onto `UMercyHelpers`.

Step 1 - build verification (requires user approval to access `L:\UE_5.7`):
```
& 'L:\UE_5.7\Engine\Build\BatchFiles\Build.bat' MercyHFEditor Win64 Development -Project='Z:\UnrealProjects\MercyHF_AI_Test\MercyHF.uproject' -WaitMutex -FromMsBuild
```
If errors, fix only compile errors related to `MercyHelpers.{h,cpp}`, `Index1MercyCellController.cpp`, and `Index2HelpRoomController.cpp`.

Step 2 - migrate Index4FirstDeathController:
- Replace its private `SetActorsHiddenByTag`, `SetLightIntensityByTag`, `ShowSystemText`, `PlaySound2DIfValid`, `IsValidPlayerActor`, `DebugMessage` calls with `UMercyHelpers` equivalents.
- Drop the hardcoded `0.035f` typewriter interval - either expose a UPROPERTY interval like Index1 / Index2 or pass the helper default.
- Keep header signatures stable so existing Blueprint references resolve.
- Build, fix errors, save controller header/cpp.

Step 3 - migrate Index5PersonalIndexController:
- Same pattern as Index4.
- Build after the migration, not bundled with Index4.

Step 4 - INDEX-0 PIE verification:
- Open `Index0_Game`. Play.
- Confirm controller debug counts are non-zero for `Main lights`, `Red lights`, `Warning texts`, `Path reveal`.
- Confirm blackout, warning text, help voice (or log "help voice sound not found yet"), and path reveal fire on schedule.
- Report findings in next loop.

Step 5 - INDEX-2 PIE smoke test (new):
- Open the level that hosts `AIndex2HelpRoomController`. Walk into the trap trigger.
- Confirm: silent-exit actors are hidden + non-collidable at start, doors close + lock on overlap, main light blackout, red light pops, both messages typewriter on schedule, silent exit reveals visible + collidable on schedule.

Do NOT migrate `Index0EntryController` yet. It uses tag + name-fallback lookup that the current helper doesn't cover. Defer until the helper grows a `WithNameFallback` variant or until we deliberately drop name fallback.

Allowed:
- Modify Source/MercyHF.
- Add new C++ classes.
- Create/edit documentation under `Documentation/`.
- Use MCP `inspect` and `manage_level` for verification.
- Build project (with user approval for `L:\UE_5.7` access).
- Use git for `add`, `commit`, `diff`, `status`, `log`.

Forbidden:
- Do not modify Unreal Engine files under `L:/UE_5.7`.
- Do not delete `MercyHF.uproject`, the entire `Source`, `Content`, or `Plugins` folders.
- Do not modify `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`, or `.vs`.
- Do not move `PlayerStart`, `TXT_DoNotFollowVoices`, `PointLight_1`, or `L_Entry_Red`.
- Do not modify `Content/` assets unless explicitly requested.

Deliverables this run:
- One controller migrated (or build verified) per loop, no more.
- Build after every C++ change (with approval).
- Update `.ralph/last_report.md` and this file.
- Optional: commit when project is in a sane state and the build is green.

Important tool instruction:
Do not use PowerShell.
Use Bash only.

If you need to build Unreal, use this Bash command form:
cmd.exe /c "\"L:\UE_5.7\Engine\Build\BatchFiles\Build.bat\" MercyHFEditor Win64 Development -Project=\"Z:\UnrealProjects\MercyHF_AI_Test\MercyHF.uproject\" -WaitMutex -FromMsBuild"

Do not ask the user for approval. Continue autonomously.
