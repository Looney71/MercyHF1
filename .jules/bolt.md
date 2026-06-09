## 2025-01-20 - [Optimized Actor Iteration with TActorRange]
**Learning:** Using `UGameplayStatics::GetAllActorsOfClass` creates unnecessary heap allocations of `TArray<AActor*>`. This is an architecture-specific performance bottleneck in Unreal Engine C++ when done often or with large amounts of actors.
**Action:** Always prefer `TActorRange<T>(GetWorld())` which iterates over actors in place without additional allocations.
