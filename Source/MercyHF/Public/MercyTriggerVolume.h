#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyTriggerVolume.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyTriggerActivatedSignature, AMercyTriggerVolume*, TriggerVolume);

UCLASS()
class MERCYHF_API AMercyTriggerVolume : public AActor
{
	GENERATED_BODY()

public:
	AMercyTriggerVolume();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* TriggerBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Trigger")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Trigger")
	bool bTriggerOnlyOnce = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Trigger")
	bool bTriggerOnPlayerOverlap = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Trigger")
	float ActivationDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Trigger")
	FVector TriggerBoxExtent = FVector(200.0f, 200.0f, 200.0f);

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* SoundToPlay = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	bool bPlaySound2D = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	FName ActorsToShowTag = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	FName ActorsToHideTag = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	FName ActorsToEnableCollisionTag = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	FName ActorsToDisableCollisionTag = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	TArray<AActor*> ActorsToShow;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	TArray<AActor*> ActorsToHide;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	TArray<AActor*> ActorsToEnableCollision;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	TArray<AActor*> ActorsToDisableCollision;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	bool bHasTriggered = false;

	FTimerHandle ActivationTimerHandle;

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
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Trigger")
	FMercyTriggerActivatedSignature OnMercyTriggerActivated;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Trigger")
	void ActivateTrigger();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Trigger")
	void ResetTrigger();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Trigger")
	void EnableTrigger();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Trigger")
	void DisableTrigger();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Trigger")
	bool HasTriggered() const;

private:
	void ExecuteActivation();
	void ApplyActorVisibilityByTag(FName Tag, bool bShouldHide);
	void ApplyActorVisibilityList(const TArray<AActor*>& Actors, bool bShouldHide);
	void ApplyActorCollisionByTag(FName Tag, bool bEnableCollision);
	void ApplyActorCollisionList(const TArray<AActor*>& Actors, bool bEnableCollision);

	bool IsValidPlayerActor(AActor* OtherActor) const;
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};