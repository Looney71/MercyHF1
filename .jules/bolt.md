## 2024-11-20 - [Optimize Actor Iteration]
**Learning:** `UGameplayStatics::GetAllActorsOfClass` copies all actor pointers into an intermediate `TArray`, which requires a large heap allocation for levels with thousands of actors. `TActorRange<AActor>(GetWorld())` achieves the exact same iteration locally without the `TArray` allocation.
**Action:** Use `TActorRange<AActor>` (along with `#include "EngineUtils.h"`) in place of `GetAllActorsOfClass` when iterating over actors during initialization or heavy processing loops.

## 2024-11-20 - [Frontend Dependencies on Backend PRs]
**Learning:** Running `pnpm install` in the `WebUI` plugin directory to perform test validations after a backend PR can silently generate and stage a new `pnpm-lock.yaml` file, causing noise in the PR.
**Action:** Always clean up untracked or unwanted lock files (`git restore --staged <file> && rm <file>`) after running `pnpm install` if no package changes were intended.
