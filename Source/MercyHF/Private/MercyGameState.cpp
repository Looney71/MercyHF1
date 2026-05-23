#include "MercyGameState.h"

#include "Net/UnrealNetwork.h"

AMercyGameState::AMercyGameState()
{
	bReplicates = true;
	UnlockedIndexes.AddUnique(TEXT("INDEX_0_ENTRY"));
}

void AMercyGameState::BeginPlay()
{
	Super::BeginPlay();

	UnlockedIndexes.AddUnique(TEXT("INDEX_0_ENTRY"));
}

void AMercyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMercyGameState, CurrentIndexName);
	DOREPLIFETIME(AMercyGameState, GroupMercyProtocolPercent);
	DOREPLIFETIME(AMercyGameState, GroupFearPercent);
	DOREPLIFETIME(AMercyGameState, GroupDeathCount);
	DOREPLIFETIME(AMercyGameState, CompletedIndexes);
	DOREPLIFETIME(AMercyGameState, UnlockedIndexes);
}

void AMercyGameState::SetCurrentIndexName(FName NewIndexName)
{
	if (NewIndexName.IsNone())
	{
		return;
	}

	CurrentIndexName = NewIndexName;
	OnCurrentIndexChanged.Broadcast(CurrentIndexName);
}

void AMercyGameState::AddGroupMercyProtocol(float Delta)
{
	GroupMercyProtocolPercent = ClampPercent(GroupMercyProtocolPercent + Delta);
	OnGroupMercyProtocolChanged.Broadcast(GroupMercyProtocolPercent);
}

void AMercyGameState::AddGroupFear(float Delta)
{
	GroupFearPercent = ClampPercent(GroupFearPercent + Delta);
	OnGroupFearChanged.Broadcast(GroupFearPercent);
}

void AMercyGameState::AddGroupDeath()
{
	++GroupDeathCount;
	OnGroupDeathCountChanged.Broadcast(GroupDeathCount);
}

void AMercyGameState::MarkIndexUnlocked(FName IndexName)
{
	if (IndexName.IsNone())
	{
		return;
	}

	UnlockedIndexes.AddUnique(IndexName);
}

void AMercyGameState::MarkIndexCompleted(FName IndexName)
{
	if (IndexName.IsNone())
	{
		return;
	}

	CompletedIndexes.AddUnique(IndexName);
	UnlockedIndexes.AddUnique(IndexName);
}

FName AMercyGameState::GetCurrentIndexName() const
{
	return CurrentIndexName;
}

float AMercyGameState::GetGroupMercyProtocolPercent() const
{
	return GroupMercyProtocolPercent;
}

float AMercyGameState::GetGroupFearPercent() const
{
	return GroupFearPercent;
}

int32 AMercyGameState::GetGroupDeathCount() const
{
	return GroupDeathCount;
}

bool AMercyGameState::IsIndexUnlocked(FName IndexName) const
{
	return UnlockedIndexes.Contains(IndexName);
}

bool AMercyGameState::IsIndexCompleted(FName IndexName) const
{
	return CompletedIndexes.Contains(IndexName);
}

TArray<FName> AMercyGameState::GetUnlockedIndexes() const
{
	return UnlockedIndexes;
}

TArray<FName> AMercyGameState::GetCompletedIndexes() const
{
	return CompletedIndexes;
}

void AMercyGameState::OnRep_CurrentIndexName()
{
	OnCurrentIndexChanged.Broadcast(CurrentIndexName);
}

void AMercyGameState::OnRep_GroupMercyProtocolPercent()
{
	OnGroupMercyProtocolChanged.Broadcast(GroupMercyProtocolPercent);
}

void AMercyGameState::OnRep_GroupFearPercent()
{
	OnGroupFearChanged.Broadcast(GroupFearPercent);
}

void AMercyGameState::OnRep_GroupDeathCount()
{
	OnGroupDeathCountChanged.Broadcast(GroupDeathCount);
}

void AMercyGameState::OnRep_CompletedIndexes()
{
}

void AMercyGameState::OnRep_UnlockedIndexes()
{
}

float AMercyGameState::ClampPercent(float Value) const
{
	return FMath::Clamp(Value, 0.0f, 100.0f);
}