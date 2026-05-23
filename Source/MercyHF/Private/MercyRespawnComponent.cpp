#include "MercyRespawnComponent.h"

#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

UMercyRespawnComponent::UMercyRespawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMercyRespawnComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!bHasCheckpoint && GetOwner())
	{
		SaveCheckpoint(TEXT("Initial"), GetOwner()->GetActorTransform());
	}
}

void UMercyRespawnComponent::SaveCheckpoint(FName CheckpointId, const FTransform& NewCheckpointTransform)
{
	CurrentCheckpointId = CheckpointId;
	CheckpointTransform = NewCheckpointTransform;
	bHasCheckpoint = true;

	OnCheckpointChanged.Broadcast(CurrentCheckpointId);

	DebugMessage(
		FString::Printf(TEXT("Checkpoint saved: %s"), *CurrentCheckpointId.ToString()),
		FColor::Green,
		3.0f
	);
}

bool UMercyRespawnComponent::RespawnOwnerAtCheckpoint()
{
	if (!bHasCheckpoint || !GetOwner())
	{
		DebugMessage(TEXT("Respawn failed: no checkpoint"), FColor::Red, 4.0f);
		return false;
	}

	GetOwner()->SetActorTransform(
		CheckpointTransform,
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);

	OnRespawned.Broadcast();

	DebugMessage(
		FString::Printf(TEXT("Respawned at checkpoint: %s"), *CurrentCheckpointId.ToString()),
		FColor::Cyan,
		4.0f
	);

	return true;
}

void UMercyRespawnComponent::ClearCheckpoint()
{
	CurrentCheckpointId = NAME_None;
	CheckpointTransform = FTransform::Identity;
	bHasCheckpoint = false;
}

bool UMercyRespawnComponent::HasCheckpoint() const
{
	return bHasCheckpoint;
}

FName UMercyRespawnComponent::GetCurrentCheckpointId() const
{
	return CurrentCheckpointId;
}

FTransform UMercyRespawnComponent::GetCheckpointTransform() const
{
	return CheckpointTransform;
}

void UMercyRespawnComponent::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}