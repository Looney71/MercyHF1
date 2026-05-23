#include "MercyAudioTrigger.h"

#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AMercyAudioTrigger::AMercyAudioTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetupAttachment(SceneRoot);
	TriggerSphere->SetSphereRadius(TriggerRadius);
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMercyAudioTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerSphere)
	{
		TriggerSphere->SetSphereRadius(TriggerRadius);
		TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AMercyAudioTrigger::HandleTriggerBeginOverlap);
	}

	if (bPlayOnBeginPlay)
	{
		PlayMercyAudio();
	}

	DebugMessage(TEXT("MercyAudioTrigger ready"), FColor::Cyan, 3.0f);
}

void AMercyAudioTrigger::HandleTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!bPlayOnPlayerOverlap)
	{
		return;
	}

	if (!IsValidPlayerActor(OtherActor))
	{
		return;
	}

	if (bPlayOnlyOnce && bHasPlayed)
	{
		return;
	}

	PlayMercyAudio();
}

void AMercyAudioTrigger::PlayMercyAudio()
{
	USoundBase* ResolvedSound = ResolveSoundAsset();

	if (!ResolvedSound)
	{
		DebugMessage(TEXT("MercyAudioTrigger: sound not found."), FColor::Orange, 5.0f);
		return;
	}

	if (bStopPreviousBeforeReplay && ActiveAudioComponent)
	{
		ActiveAudioComponent->Stop();
		ActiveAudioComponent = nullptr;
	}

	if (bUse2DSound)
	{
		ActiveAudioComponent = UGameplayStatics::SpawnSound2D(
			this,
			ResolvedSound,
			VolumeMultiplier,
			PitchMultiplier,
			StartTime
		);
	}
	else
	{
		ActiveAudioComponent = UGameplayStatics::SpawnSoundAtLocation(
			this,
			ResolvedSound,
			GetActorLocation(),
			GetActorRotation(),
			VolumeMultiplier,
			PitchMultiplier,
			StartTime
		);
	}

	bHasPlayed = true;

	DebugMessage(TEXT("MercyAudioTrigger: audio played."), FColor::Green, 4.0f);
}

void AMercyAudioTrigger::StopMercyAudio()
{
	if (ActiveAudioComponent)
	{
		ActiveAudioComponent->Stop();
		ActiveAudioComponent = nullptr;
	}

	DebugMessage(TEXT("MercyAudioTrigger: audio stopped."), FColor::Yellow, 3.0f);
}

void AMercyAudioTrigger::ResetTrigger()
{
	bHasPlayed = false;

	DebugMessage(TEXT("MercyAudioTrigger: trigger reset."), FColor::Cyan, 3.0f);
}

USoundBase* AMercyAudioTrigger::ResolveSoundAsset()
{
	if (SoundToPlay)
	{
		return SoundToPlay;
	}

	if (SoundAssetPath.IsEmpty())
	{
		return nullptr;
	}

	SoundToPlay = Cast<USoundBase>(
		StaticLoadObject(USoundBase::StaticClass(), nullptr, *SoundAssetPath)
	);

	return SoundToPlay;
}

bool AMercyAudioTrigger::IsValidPlayerActor(AActor* OtherActor) const
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

void AMercyAudioTrigger::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}