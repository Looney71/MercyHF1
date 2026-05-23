#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyScareEvent.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMercyScareEventActivated);

UCLASS()
class MERCYHF_API AMercyScareEvent : public AActor
{
	GENERATED_BODY()

public:
	AMercyScareEvent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* TriggerBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Scare")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Scare")
	bool bTriggerOnlyOnce = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Scare")
	bool bTriggerOnPlayerOverlap = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Scare")
	float ActivationDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Scare")
	bool bAutoHideShownActors = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Scare", meta = (EditCondition = "bAutoHideShownActors", ClampMin = "0.01"))
	float AutoHideDelay = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Scare")
	FVector TriggerBoxExtent = FVector(200.0f, 200.0f, 200.0f);

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* ScareSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	bool bPlaySound2D = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	TArray<AActor*> ActorsToShow;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	TArray<AActor*> ActorsToHide;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	FName ActorsToShowTag = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	FName ActorsToHideTag = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Collision")
	bool bEnableCollisionOnShownActors = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Collision")
	bool bDisableCollisionOnHiddenActors = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	bool bHasTriggered = false;

	FTimerHandle ActivationTimerHandle;
	FTimerHandle AutoHideTimerHandle;

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
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Scare")
	FMercyScareEventActivated OnScareActivated;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Scare")
	void ActivateScare();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Scare")
	void ResetScare();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Scare")
	void EnableScare();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Scare")
	void DisableScare();

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Scare")
	bool HasTriggered() const;

private:
	void ExecuteScare();
	void AutoHideShownActors();

	void SetActorsHidden(const TArray<AActor*>& Actors, bool bShouldHide);
	void SetActorsHiddenByTag(FName Tag, bool bShouldHide);
	void SetActorListCollision(const TArray<AActor*>& Actors, bool bEnableCollision);
	void SetActorCollisionByTag(FName Tag, bool bEnableCollision);

	bool IsValidPlayerActor(AActor* OtherActor) const;

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};