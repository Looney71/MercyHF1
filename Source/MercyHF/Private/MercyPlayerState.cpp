#include "MercyPlayerState.h"

#include "Net/UnrealNetwork.h"

AMercyPlayerState::AMercyPlayerState()
{
	bReplicates = true;
}

void AMercyPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AMercyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMercyPlayerState, PlayerFearPercent);
	DOREPLIFETIME(AMercyPlayerState, PlayerMercyPercent);
	DOREPLIFETIME(AMercyPlayerState, PlayerResistancePercent);
	DOREPLIFETIME(AMercyPlayerState, PlayerDeathCount);
	DOREPLIFETIME(AMercyPlayerState, bPlayerReady);
	DOREPLIFETIME(AMercyPlayerState, bPlayerMarked);
	DOREPLIFETIME(AMercyPlayerState, CurrentIndexName);
}

void AMercyPlayerState::SetPlayerReady(bool bNewReady)
{
	bPlayerReady = bNewReady;
	OnPlayerReadyChanged.Broadcast(bPlayerReady);
}

void AMercyPlayerState::SetPlayerMarked(bool bNewMarked)
{
	bPlayerMarked = bNewMarked;
	OnPlayerMarkedChanged.Broadcast(bPlayerMarked);
}

void AMercyPlayerState::SetCurrentIndexName(FName NewIndexName)
{
	CurrentIndexName = NewIndexName;
	OnRep_CurrentIndexName();
}

void AMercyPlayerState::AddFear(float Delta)
{
	PlayerFearPercent = ClampPercent(PlayerFearPercent + Delta);
	OnPlayerFearChanged.Broadcast(PlayerFearPercent);
}

void AMercyPlayerState::AddMercy(float Delta)
{
	PlayerMercyPercent = ClampPercent(PlayerMercyPercent + Delta);
	OnPlayerMercyChanged.Broadcast(PlayerMercyPercent);
}

void AMercyPlayerState::AddResistance(float Delta)
{
	PlayerResistancePercent = ClampPercent(PlayerResistancePercent + Delta);
	OnPlayerResistanceChanged.Broadcast(PlayerResistancePercent);
}

void AMercyPlayerState::AddDeath()
{
	++PlayerDeathCount;
	OnPlayerDeathCountChanged.Broadcast(PlayerDeathCount);
}

void AMercyPlayerState::ResetPlayerIndexStats()
{
	PlayerFearPercent = 0.0f;
	PlayerMercyPercent = 0.0f;
	PlayerResistancePercent = 0.0f;
	PlayerDeathCount = 0;
	bPlayerMarked = false;

	OnPlayerFearChanged.Broadcast(PlayerFearPercent);
	OnPlayerMercyChanged.Broadcast(PlayerMercyPercent);
	OnPlayerResistanceChanged.Broadcast(PlayerResistancePercent);
	OnPlayerDeathCountChanged.Broadcast(PlayerDeathCount);
	OnPlayerMarkedChanged.Broadcast(bPlayerMarked);
}

float AMercyPlayerState::GetPlayerFearPercent() const
{
	return PlayerFearPercent;
}

float AMercyPlayerState::GetPlayerMercyPercent() const
{
	return PlayerMercyPercent;
}

float AMercyPlayerState::GetPlayerResistancePercent() const
{
	return PlayerResistancePercent;
}

int32 AMercyPlayerState::GetPlayerDeathCount() const
{
	return PlayerDeathCount;
}

bool AMercyPlayerState::IsPlayerReady() const
{
	return bPlayerReady;
}

bool AMercyPlayerState::IsPlayerMarked() const
{
	return bPlayerMarked;
}

FName AMercyPlayerState::GetCurrentIndexName() const
{
	return CurrentIndexName;
}

void AMercyPlayerState::OnRep_PlayerFearPercent()
{
	OnPlayerFearChanged.Broadcast(PlayerFearPercent);
}

void AMercyPlayerState::OnRep_PlayerMercyPercent()
{
	OnPlayerMercyChanged.Broadcast(PlayerMercyPercent);
}

void AMercyPlayerState::OnRep_PlayerResistancePercent()
{
	OnPlayerResistanceChanged.Broadcast(PlayerResistancePercent);
}

void AMercyPlayerState::OnRep_PlayerDeathCount()
{
	OnPlayerDeathCountChanged.Broadcast(PlayerDeathCount);
}

void AMercyPlayerState::OnRep_bPlayerReady()
{
	OnPlayerReadyChanged.Broadcast(bPlayerReady);
}

void AMercyPlayerState::OnRep_bPlayerMarked()
{
	OnPlayerMarkedChanged.Broadcast(bPlayerMarked);
}

void AMercyPlayerState::OnRep_CurrentIndexName()
{
}

float AMercyPlayerState::ClampPercent(float Value) const
{
	return FMath::Clamp(Value, 0.0f, 100.0f);
}