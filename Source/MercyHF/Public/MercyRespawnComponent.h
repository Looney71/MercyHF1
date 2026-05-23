#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MercyRespawnComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyCheckpointChanged, FName, CheckpointId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMercyRespawned);

UCLASS(ClassGroup = (MercyMaze), meta = (BlueprintSpawnableComponent))
class MERCYHF_API UMercyRespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMercyRespawnComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Respawn")
	FName CurrentCheckpointId = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Respawn")
	FTransform CheckpointTransform;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Respawn")
	bool bHasCheckpoint = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

public:
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Respawn")
	FMercyCheckpointChanged OnCheckpointChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Respawn")
	FMercyRespawned OnRespawned;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Respawn")
	void SaveCheckpoint(FName CheckpointId, const FTransform& NewCheckpointTransform);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Respawn")
	bool RespawnOwnerAtCheckpoint();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Respawn")
	void ClearCheckpoint();

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Respawn")
	bool HasCheckpoint() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Respawn")
	FName GetCurrentCheckpointId() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Respawn")
	FTransform GetCheckpointTransform() const;

private:
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};