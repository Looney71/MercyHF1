#include "MercyObjectiveManager.h"

#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

AMercyObjectiveManager::AMercyObjectiveManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AMercyObjectiveManager::BeginPlay()
{
	Super::BeginPlay();
	DebugMessage(TEXT("MercyObjectiveManager ready"), FColor::Cyan, 3.0f);
}

void AMercyObjectiveManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMercyObjectiveManager, Objectives);
}

void AMercyObjectiveManager::AddObjective(FName ObjectiveId, FText ObjectiveText, bool bRequiredToCompleteIndex, bool bActivateImmediately)
{
	if (ObjectiveId.IsNone())
	{
		return;
	}

	if (FindObjectiveIndex(ObjectiveId) != INDEX_NONE)
	{
		return;
	}

	FMercyObjectiveData NewObjective;
	NewObjective.ObjectiveId = ObjectiveId;
	NewObjective.ObjectiveText = ObjectiveText;
	NewObjective.bRequiredToCompleteIndex = bRequiredToCompleteIndex;
	NewObjective.State = bActivateImmediately ? EMercyObjectiveState::Active : EMercyObjectiveState::Inactive;

	Objectives.Add(NewObjective);

	if (bActivateImmediately)
	{
		OnObjectiveActivated.Broadcast(ObjectiveId, ObjectiveText);
	}

	DebugMessage(FString::Printf(TEXT("Objective added: %s"), *ObjectiveId.ToString()), FColor::Green, 3.0f);
}

void AMercyObjectiveManager::ActivateObjective(FName ObjectiveId)
{
	const int32 Index = FindObjectiveIndex(ObjectiveId);

	if (Index == INDEX_NONE)
	{
		return;
	}

	Objectives[Index].State = EMercyObjectiveState::Active;
	OnObjectiveActivated.Broadcast(Objectives[Index].ObjectiveId, Objectives[Index].ObjectiveText);
}

void AMercyObjectiveManager::CompleteObjective(FName ObjectiveId)
{
	const int32 Index = FindObjectiveIndex(ObjectiveId);

	if (Index == INDEX_NONE)
	{
		return;
	}

	Objectives[Index].State = EMercyObjectiveState::Completed;
	OnObjectiveCompleted.Broadcast(Objectives[Index].ObjectiveId, Objectives[Index].ObjectiveText);

	DebugMessage(FString::Printf(TEXT("Objective completed: %s"), *ObjectiveId.ToString()), FColor::Green, 3.0f);

	CheckAllRequiredObjectivesComplete();
}

void AMercyObjectiveManager::FailObjective(FName ObjectiveId)
{
	const int32 Index = FindObjectiveIndex(ObjectiveId);

	if (Index == INDEX_NONE)
	{
		return;
	}

	Objectives[Index].State = EMercyObjectiveState::Failed;
	OnObjectiveFailed.Broadcast(Objectives[Index].ObjectiveId, Objectives[Index].ObjectiveText);

	DebugMessage(FString::Printf(TEXT("Objective failed: %s"), *ObjectiveId.ToString()), FColor::Red, 3.0f);
}

void AMercyObjectiveManager::ResetObjectives()
{
	Objectives.Empty();
	DebugMessage(TEXT("Objectives reset"), FColor::Yellow, 3.0f);
}

bool AMercyObjectiveManager::IsObjectiveComplete(FName ObjectiveId) const
{
	const int32 Index = FindObjectiveIndex(ObjectiveId);

	return Index != INDEX_NONE && Objectives[Index].State == EMercyObjectiveState::Completed;
}

bool AMercyObjectiveManager::AreAllRequiredObjectivesComplete() const
{
	for (const FMercyObjectiveData& Objective : Objectives)
	{
		if (Objective.bRequiredToCompleteIndex && Objective.State != EMercyObjectiveState::Completed)
		{
			return false;
		}
	}

	return true;
}

FText AMercyObjectiveManager::GetObjectiveText(FName ObjectiveId) const
{
	const int32 Index = FindObjectiveIndex(ObjectiveId);

	if (Index == INDEX_NONE)
	{
		return FText::GetEmpty();
	}

	return Objectives[Index].ObjectiveText;
}

TArray<FMercyObjectiveData> AMercyObjectiveManager::GetObjectives() const
{
	return Objectives;
}

void AMercyObjectiveManager::OnRep_Objectives()
{
	for (const FMercyObjectiveData& Objective : Objectives)
	{
		BroadcastObjectiveState(Objective);
	}
}

int32 AMercyObjectiveManager::FindObjectiveIndex(FName ObjectiveId) const
{
	for (int32 Index = 0; Index < Objectives.Num(); ++Index)
	{
		if (Objectives[Index].ObjectiveId == ObjectiveId)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

void AMercyObjectiveManager::BroadcastObjectiveState(const FMercyObjectiveData& Objective)
{
	if (Objective.State == EMercyObjectiveState::Active)
	{
		OnObjectiveActivated.Broadcast(Objective.ObjectiveId, Objective.ObjectiveText);
	}
	else if (Objective.State == EMercyObjectiveState::Completed)
	{
		OnObjectiveCompleted.Broadcast(Objective.ObjectiveId, Objective.ObjectiveText);
	}
	else if (Objective.State == EMercyObjectiveState::Failed)
	{
		OnObjectiveFailed.Broadcast(Objective.ObjectiveId, Objective.ObjectiveText);
	}
}

void AMercyObjectiveManager::CheckAllRequiredObjectivesComplete()
{
	if (AreAllRequiredObjectivesComplete())
	{
		OnAllRequiredObjectivesCompleted.Broadcast();
		DebugMessage(TEXT("All required objectives completed"), FColor::Green, 4.0f);
	}
}

void AMercyObjectiveManager::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}