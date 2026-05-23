#include "MercyCheckpointActor.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MercyRespawnComponent.h"
#include "Sound/SoundBase.h"

AMercyCheckpointActor::AMercyCheckpointActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CheckpointBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckpointBox"));
	CheckpointBox->SetupAttachment(SceneRoot);
	CheckpointBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CheckpointBox->SetCollisionObjectType(ECC_WorldDynamic);
	CheckpointBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CheckpointBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMercyCheckpointActor::BeginPlay()
{
	Super::BeginPlay();

	if (CheckpointBox)
	{
		CheckpointBox->SetBoxExtent(CheckpointBoxExtent);
		CheckpointBox->OnComponentBeginOverlap.AddDynamic(this, &AMercyCheckpointActor::HandleCheckpointBeginOverlap);
	}

	DebugMessage(TEXT("MercyCheckpointActor ready"), FColor::Cyan, 3.0f);
}

void AMercyCheckpointActor::HandleCheckpointBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!bActivateOnPlayerOverlap)
	{
		return;
	}

	if (!IsValidPlayerActor(OtherActor))
	{
		return;
	}

	ActivateCheckpointForActor(OtherActor);
}

void AMercyCheckpointActor::ActivateCheckpointForActor(AActor* PlayerActor)
{
	if (!PlayerActor)
	{
		return;
	}

	if (bActivateOnlyOncePerPlayer && ActivatedPlayers.Contains(PlayerActor))
	{
		return;
	}

	UMercyRespawnComponent* RespawnComponent = PlayerActor->FindComponentByClass<UMercyRespawnComponent>();

	if (!RespawnComponent)
	{
		DebugMessage(TEXT("Checkpoint: player has no MercyRespawnComponent yet"), FColor::Orange, 4.0f);
		return;
	}

	RespawnComponent->SaveCheckpoint(CheckpointId, GetRespawnTransform());

	ActivatedPlayers.AddUnique(PlayerActor);

	if (ActivationSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ActivationSound, GetActorLocation());
	}

	OnCheckpointActivated.Broadcast(PlayerActor);

	DebugMessage(
		FString::Printf(TEXT("Checkpoint activated: %s"), *CheckpointId.ToString()),
		FColor::Green,
		4.0f
	);
}

void AMercyCheckpointActor::ResetCheckpoint()
{
	ActivatedPlayers.Empty();

	DebugMessage(TEXT("Checkpoint reset"), FColor::Cyan, 3.0f);
}

FName AMercyCheckpointActor::GetCheckpointId() const
{
	return CheckpointId;
}

bool AMercyCheckpointActor::IsValidPlayerActor(AActor* OtherActor) const
{
	if (!OtherActor)
	{
		return false;
	}

	if (OtherActor->IsA<APawn>())
	{
		return true;
	}

	return OtherActor->ActorHasTag(TEXT("Player"));
}

FTransform AMercyCheckpointActor::GetRespawnTransform() const
{
	if (bUseCustomRespawnTransform)
	{
		return CustomRespawnTransform;
	}

	return GetActorTransform();
}

void AMercyCheckpointActor::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}