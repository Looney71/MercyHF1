#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyCoopPuzzleController.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMercyCoopPuzzleSolved);

UCLASS()
class MERCYHF_API AMercyCoopPuzzleController : public AActor
{
	GENERATED_BODY()

public:
	AMercyCoopPuzzleController();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* PuzzleZone = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Coop")
	int32 RequiredPlayers = 2;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Coop")
	bool bSolveOnlyOnce = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Coop")
	float SolveDelay = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Coop")
	FVector PuzzleZoneExtent = FVector(300.0f, 300.0f, 200.0f);

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	TArray<AActor*> ActorsToShowOnSolved;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	TArray<AActor*> ActorsToHideOnSolved;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	FName ActorsToShowOnSolvedTag = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Targets")
	FName ActorsToHideOnSolvedTag = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* SolvedSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	UPROPERTY()
	TArray<AActor*> PlayersInside;

	bool bSolved = false;

	FTimerHandle SolveTimerHandle;

	UFUNCTION()
	void HandleZoneBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void HandleZoneEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex
	);

public:
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Coop")
	FMercyCoopPuzzleSolved OnPuzzleSolved;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Coop")
	void ResetPuzzle();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Coop")
	void ForceSolvePuzzle();

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Coop")
	bool IsPuzzleSolved() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Coop")
	int32 GetPlayersInsideCount() const;

private:
	void EvaluatePuzzleState();
	void SolvePuzzle();
	void ApplySolvedActions();
	void SetActorsHidden(const TArray<AActor*>& Actors, bool bShouldHide);
	void SetActorsHiddenByTag(FName Tag, bool bShouldHide);

	bool IsValidPlayerActor(AActor* OtherActor) const;
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};