## 2024-06-04 - Unreal Engine Actor Iteration Bottleneck
**Learning:** `UGameplayStatics::GetAllActorsOfClass` allocates a new `TArray` on the heap every time it's called, causing memory fragmentation and allocation overhead during initialization or tick.
**Action:** Always prefer `TActorRange<T>(GetWorld())` (requires `#include "EngineUtils.h"`) for lightweight, heap-allocation-free actor iteration in C++ when you just need to loop over actors without storing the entire list long-term.
