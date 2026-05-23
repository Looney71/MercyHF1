#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyAudioTrigger.generated.h"

class USceneComponent;
class USphereComponent;
class USoundBase;
class UAudioComponent;

UCLASS()
class MERCYHF_API AMercyAudioTrigger : public AActor
{
	GENERATED_BODY()

public:
	AMercyAudioTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USphereComponent* TriggerSphere = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* SoundToPlay = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	FString SoundAssetPath = TEXT("/Game/MercyMaze/Audio/SFX_Help.SFX_Help");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	bool bPlayOnBeginPlay = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	bool bPlayOnPlayerOverlap = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	bool bPlayOnlyOnce = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	bool bUse2DSound = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	bool bStopPreviousBeforeReplay = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio", meta = (ClampMin = "0.0"))
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio", meta = (ClampMin = "0.1"))
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio", meta = (ClampMin = "0.0"))
	float StartTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Trigger", meta = (ClampMin = "50.0"))
	float TriggerRadius = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	UPROPERTY()
	UAudioComponent* ActiveAudioComponent = nullptr;

	bool bHasPlayed = false;

	UFUNCTION()
	void HandleTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Audio")
	void PlayMercyAudio();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Audio")
	void StopMercyAudio();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Audio")
	void ResetTrigger();

private:
	USoundBase* ResolveSoundAsset();
	bool IsValidPlayerActor(AActor* OtherActor) const;

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};