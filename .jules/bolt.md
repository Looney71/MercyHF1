## 2024-06-25 - Avoid UGameplayStatics::GetAllActorsOfClass for general iteration
**Learning:** In this Unreal Engine C++ project, using `UGameplayStatics::GetAllActorsOfClass` with `AActor::StaticClass()` causes massive unnecessary `TArray` heap allocations because it stores every single actor in the level in an array just to iterate through them.
**Action:** Always prefer `TActorRange<T>(GetWorld())` (requires `#include "EngineUtils.h"`) for zero-allocation actor iteration when traversing all actors in a level.
