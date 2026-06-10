
## 2024-05-18 - [Optimization] Avoid `GetAllActorsOfClass` Heap Allocations
**Learning:** `UGameplayStatics::GetAllActorsOfClass` creates a `TArray` heap allocation every time it's called, which is inefficient for frame-time logic or heavy iterative caching.
**Action:** Prefer `TActorRange<AActor>(GetWorld())` which operates as a direct engine-level iterator. Requires `#include "EngineUtils.h"`.
