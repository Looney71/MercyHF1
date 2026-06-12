## 2025-02-12 - Using TActorRange in Unreal Engine C++
**Learning:** TActorRange is significantly faster than UGameplayStatics::GetAllActorsOfClass because it immediately yields an iterator over actors in the World, skipping the allocation and population of a large `TArray` instance on the heap.
**Action:** When gathering actors for processing in large maps during initialization (e.g. `BeginPlay` caches), prefer `TActorRange<AActor>(GetWorld())` inside range-based loops to minimize overhead and improve load times.
