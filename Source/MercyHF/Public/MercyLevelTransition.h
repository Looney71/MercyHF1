#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyLevelTransition.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyLevelTransitionStarted, FName, TargetLevelName);

UCLASS()
class MERCYHF_API AMercyLevelTransition : public AActor
{
	GENERATED_BODY()

public:
	AMercyLevelTransition();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* TriggerBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Transition")
	FName TargetLevelName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Transition")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Transition")
	bool bTriggerOnlyOnce = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Transition")
	bool bTriggerOnPlayerOverlap = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Transition")
	float TransitionDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Transition")
	FVector TriggerBoxExtent = FVector(200.0f, 200.0f, 200.0f);

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* TransitionSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	bool bHasTriggered = false;

	FTimerHandle TransitionTimerHandle;

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
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Transition")
	FMercyLevelTransitionStarted OnTransitionStarted;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Transition")
	void StartTransition();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Transition")
	void SetTargetLevel(FName NewTargetLevelName);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Transition")
	void EnableTransition();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Transition")
	void DisableTransition();

private:
	void ExecuteLevelOpen();
	bool IsValidPlayerActor(AActor* OtherActor) const;
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};