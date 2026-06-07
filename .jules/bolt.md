## 2024-06-07 - Avoid TArray Allocation for Actor Iteration
**Learning:** Using `UGameplayStatics::GetAllActorsOfClass` to find a single actor creates a massive unnecessary `TArray` heap allocation and forces full iteration of all actors in the world, even if the target is found early.
**Action:** Always prefer `TActorRange<T>(GetWorld())` (with `#include "EngineUtils.h"`) for actor searches, especially when an early return is possible, to enable lazy iteration and avoid heap allocations.
