## 2024-06-02 - Avoid UGameplayStatics::GetAllActorsOfClass for broad queries
**Learning:** `UGameplayStatics::GetAllActorsOfClass` with `AActor::StaticClass()` allocates a massive `TArray` on the heap to store pointers to every single actor in the level, which is incredibly slow and memory-intensive for large maps.
**Action:** Always prefer `TActorRange<T>(GetWorld())` for iterating over actors in Unreal Engine C++. It acts as a zero-allocation iterator and is significantly faster when filtering through a large number of scene actors. Requires `#include "EngineUtils.h"`.
