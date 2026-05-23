#include "MercyFearZone.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MercyProtocolComponent.h"
#include "Sound/SoundBase.h"

AMercyFearZone::AMercyFearZone()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	FearZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FearZoneBox"));
	FearZoneBox->SetupAttachment(SceneRoot);
	FearZoneBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FearZoneBox->SetCollisionObjectType(ECC_WorldDynamic);
	FearZoneBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	FearZoneBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMercyFearZone::BeginPlay()
{
	Super::BeginPlay();

	if (FearZoneBox)
	{
		FearZoneBox->SetBoxExtent(FearZoneExtent);
		FearZoneBox->OnComponentBeginOverlap.AddDynamic(this, &AMercyFearZone::HandleZoneBeginOverlap);
	}

	DebugMessage(TEXT("MercyFearZone ready"), FColor::Cyan, 3.0f);
}

void AMercyFearZone::HandleZoneBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!bEnabled)
	{
		return;
	}

	if (bTriggerOnlyOnce && bHasTriggered)
	{
		return;
	}

	if (!IsValidPlayerActor(OtherActor))
	{
		return;
	}

	bHasTriggered = true;

	ApplyFearToActor(OtherActor);

	if (FearSound)
	{
		if (bPlaySound2D)
		{
			UGameplayStatics::PlaySound2D(this, FearSound);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(this, FearSound, GetActorLocation());
		}
	}

	OnFearZoneTriggered.Broadcast(OtherActor);

	DebugMessage(TEXT("Fear zone triggered"), FColor::Red, 4.0f);
}

void AMercyFearZone::EnableFearZone()
{
	bEnabled = true;
}

void AMercyFearZone::DisableFearZone()
{
	bEnabled = false;
}

void AMercyFearZone::ResetFearZone()
{
	bHasTriggered = false;
}

void AMercyFearZone::ApplyFearToActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	UMercyProtocolComponent* ProtocolComponent = Actor->FindComponentByClass<UMercyProtocolComponent>();

	if (!ProtocolComponent)
	{
		DebugMessage(TEXT("Fear zone: actor has no MercyProtocolComponent yet"), FColor::Orange, 4.0f);
		return;
	}

	ProtocolComponent->AddFearPercent(FearDelta);
	ProtocolComponent->AddMercyProtocolPercent(MercyProtocolDelta);
	ProtocolComponent->AddResistancePercent(ResistanceDelta);
}

bool AMercyFearZone::IsValidPlayerActor(AActor* OtherActor) const
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

void AMercyFearZone::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}