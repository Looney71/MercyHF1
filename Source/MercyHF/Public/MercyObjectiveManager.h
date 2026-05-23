#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyObjectiveManager.generated.h"

UENUM(BlueprintType)
enum class EMercyObjectiveState : uint8
{
	Inactive UMETA(DisplayName = "Inactive"),
	Active UMETA(DisplayName = "Active"),
	Completed UMETA(DisplayName = "Completed"),
	Failed UMETA(DisplayName = "Failed")
};

USTRUCT(BlueprintType)
struct FMercyObjectiveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Objective")
	FName ObjectiveId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Objective")
	FText ObjectiveText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Objective")
	EMercyObjectiveState State = EMercyObjectiveState::Inactive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Objective")
	bool bRequiredToCompleteIndex = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMercyObjectiveEvent, FName, ObjectiveId, FText, ObjectiveText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMercyAllObjectivesCompleted);

UCLASS()
class MERCYHF_API AMercyObjectiveManager : public AActor
{
	GENERATED_BODY()

public:
	AMercyObjectiveManager();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Objectives, Category = "Mercy Maze|Objectives")
	TArray<FMercyObjectiveData> Objectives;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

public:
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Objectives")
	FMercyObjectiveEvent OnObjectiveActivated;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Objectives")
	FMercyObjectiveEvent OnObjectiveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Objectives")
	FMercyObjectiveEvent OnObjectiveFailed;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Objectives")
	FMercyAllObjectivesCompleted OnAllRequiredObjectivesCompleted;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Objectives")
	void AddObjective(FName ObjectiveId, FText ObjectiveText, bool bRequiredToCompleteIndex = true, bool bActivateImmediately = true);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Objectives")
	void ActivateObjective(FName ObjectiveId);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Objectives")
	void CompleteObjective(FName ObjectiveId);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Objectives")
	void FailObjective(FName ObjectiveId);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Objectives")
	void ResetObjectives();

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Objectives")
	bool IsObjectiveComplete(FName ObjectiveId) const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Objectives")
	bool AreAllRequiredObjectivesComplete() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Objectives")
	FText GetObjectiveText(FName ObjectiveId) const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Objectives")
	TArray<FMercyObjectiveData> GetObjectives() const;

private:
	UFUNCTION()
	void OnRep_Objectives();

	int32 FindObjectiveIndex(FName ObjectiveId) const;
	void BroadcastObjectiveState(const FMercyObjectiveData& Objective);
	void CheckAllRequiredObjectivesComplete();
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};