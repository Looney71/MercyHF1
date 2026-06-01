## 2024-06-01 - TArray Allocation Bottleneck in UE5
**Learning:** Found a widespread pattern of using `UGameplayStatics::GetAllActorsOfClass` which forces an expensive `TArray` heap allocation for every iteration across the entire map, specifically impacting level controllers on `BeginPlay`.
**Action:** Replaced with `TActorRange<AActor>(GetWorld())` to iterate in place, significantly reducing memory spikes during initialization without altering logic.
