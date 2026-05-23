#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyCheckpointActor.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyCheckpointActivated, AActor*, PlayerActor);

UCLASS()
class MERCYHF_API AMercyCheckpointActor : public AActor
{
	GENERATED_BODY()

public:
	AMercyCheckpointActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* CheckpointBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Checkpoint")
	FName CheckpointId = TEXT("Checkpoint");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Checkpoint")
	bool bActivateOnPlayerOverlap = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Checkpoint")
	bool bActivateOnlyOncePerPlayer = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Checkpoint")
	bool bUseCustomRespawnTransform = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Checkpoint", meta = (EditCondition = "bUseCustomRespawnTransform"))
	FTransform CustomRespawnTransform;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Checkpoint")
	FVector CheckpointBoxExtent = FVector(250.0f, 250.0f, 200.0f);

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* ActivationSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	UPROPERTY()
	TArray<AActor*> ActivatedPlayers;

	UFUNCTION()
	void HandleCheckpointBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Checkpoint")
	FMercyCheckpointActivated OnCheckpointActivated;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Checkpoint")
	void ActivateCheckpointForActor(AActor* PlayerActor);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Checkpoint")
	void ResetCheckpoint();

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Checkpoint")
	FName GetCheckpointId() const;

private:
	bool IsValidPlayerActor(AActor* OtherActor) const;
	FTransform GetRespawnTransform() const;

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};