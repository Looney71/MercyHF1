## 2024-05-23 - Avoid TArray heap allocations with TActorRange
**Learning:** `UGameplayStatics::GetAllActorsOfClass` internally creates and returns a `TArray`, which causes unnecessary heap allocations. Using `TActorRange<T>(GetWorld())` inside a range-based for loop is a more performant alternative for iterating over actors in Unreal Engine, as it avoids array allocations.
**Action:** Prefer using `TActorRange` in range-based for loops instead of `GetAllActorsOfClass` when we only need to iterate over actors and do not need to check the array size or return an array.
