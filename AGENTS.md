# MercyHF Unreal Project Rules

You are assisting with an Unreal Engine C++ horror game project.

Hard rules:
- Do not delete assets, maps, Blueprints, plugins, or project settings.
- Do not modify Binaries, Intermediate, Saved, DerivedDataCache, or .vs.
- Do not move PlayerStart.
- Do not move TXT_DoNotFollowVoices.
- Do not move PointLight_1.
- Do not move L_Entry_Red.
- Do not modify Content assets unless explicitly requested.
- Prefer small, reversible C++ changes.
- Build after C++ changes.
- If build fails, fix only compile errors related to the last change.
- Use Unreal MCP only for verification unless explicitly asked to edit level actors.
- Stop and report if an action is ambiguous.

Current INDEX-0 setup:
- Level: Index0_Game
- Main entry light tag: ENTRY_MAIN_LIGHT
- Red entry light tag: ENTRY_RED_LIGHT
- Warning text tags: ENTRY_WARNING_TEXT and SYSTEM_TEXT
- Controller Blueprint should inherit from C++ Index0EntryController.
