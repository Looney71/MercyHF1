#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Index4FirstDeathController.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;

UCLASS()
class MERCYHF_API AIndex4FirstDeathController : public AActor
{
	GENERATED_BODY()

public:
	AIndex4FirstDeathController();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* DeathTrigger = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	bool bTriggerOnlyOnce = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	FVector DeathTriggerExtent = FVector(350.0f, 350.0f, 220.0f);

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	FName SystemTextTag = TEXT("SYSTEM_TEXT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	FName FakeExitTag = TEXT("INDEX4_FAKE_EXIT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	FName MonsterRevealTag = TEXT("INDEX4_MONSTER");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	float ChaseStartDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	float TerminationDelay = 6.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	float RespawnDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	USoundBase* ChaseSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 4")
	USoundBase* DeathSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	bool bHasTriggered = false;

	TWeakObjectPtr<AActor> PendingVictim;

	FTimerHandle ChaseTimerHandle;
	FTimerHandle TerminationTimerHandle;
	FTimerHandle RespawnTimerHandle;

	UFUNCTION()
	void HandleDeathTriggerOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Index 4")
	void StartFirstDeath(AActor* VictimActor);

private:
	void StartChase();
	void TerminateSubject();
	void RestoreSubject();

	void ShowSystemText(const FString& Message, float AutoHideAfter = 0.0f);
	void SetActorsHiddenByTag(FName Tag, bool bShouldHide);
	void PlaySound2DIfValid(USoundBase* Sound);
	bool IsValidPlayerActor(AActor* OtherActor) const;
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};