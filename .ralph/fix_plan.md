# MercyHF Compile Fix Sprint

Use claude-sonnet-4 only.

Task:
Fix only the compile errors from the latest build.

Latest build errors:
1. Index0EntryController.cpp uses FColor::Gray, but FColor has no Gray member.
   Fix by replacing FColor::Gray with FColor(128, 128, 128) or another valid FColor.

2. Index4FirstDeathController.cpp and Index5PersonalIndexController.cpp call:
   UMercyHelpers::ShowSystemText(...)
   but UMercyHelpers has no ShowSystemText function.
   Add a proper reusable UMercyHelpers::ShowSystemText function to MercyHelpers.h/.cpp OR revert those call sites to the local controller ShowSystemText implementation.
   Prefer adding the helper if it matches existing MercySystemTextActor usage.

3. Index4FirstDeathController.cpp and Index5PersonalIndexController.cpp call:
   UMercyHelpers::DebugMessage(Message, Color, Duration, bShowDebugMessages)
   but the helper signature is:
   DebugMessage(bool bShowOnScreen, const FString& Message, const FColor& Color, float Duration)
   Fix the call order to:
   UMercyHelpers::DebugMessage(bShowDebugMessages, Message, Color, Duration)

Rules:
- Do not add new gameplay features.
- Do not modify maps.
- Do not modify Blueprints.
- Do not touch Content.
- Do not modify Engine files.
- Do not use PowerShell.
- Use Bash only.
- Fix only the C++ compile errors listed above.
- After changes, write .ralph/last_report.md with exactly what was fixed.
- Stop after the compile fixes. Do not continue to new tasks.
