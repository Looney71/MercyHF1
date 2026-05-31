## 2024-05-23 - Avoid TArray allocations during actor iteration
**Learning:** `UGameplayStatics::GetAllActorsOfClass` allocates a new `TArray` every time it is called, which causes unnecessary heap allocations. This can be a notable performance hit if called frequently or in levels with many actors.
**Action:** When you only need to iterate over actors of a specific class (and don't strictly require an array copy), use `TActorRange<T>(GetWorld())` from `#include "EngineUtils.h"` instead. This creates an iterator that does not allocate heap memory.
