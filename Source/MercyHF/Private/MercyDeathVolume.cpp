#include "MercyDeathVolume.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MercyProtocolComponent.h"
#include "MercyRespawnComponent.h"
#include "MercySystemTextActor.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AMercyDeathVolume::AMercyDeathVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DeathBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DeathBox"));
	DeathBox->SetupAttachment(SceneRoot);
	DeathBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DeathBox->SetCollisionObjectType(ECC_WorldDynamic);
	DeathBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	DeathBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMercyDeathVolume::BeginPlay()
{
	Super::BeginPlay();

	if (DeathBox)
	{
		DeathBox->SetBoxExtent(DeathBoxExtent);
		DeathBox->OnComponentBeginOverlap.AddDynamic(this, &AMercyDeathVolume::HandleDeathBeginOverlap);
	}

	DebugMessage(TEXT("MercyDeathVolume ready"), FColor::Cyan, 3.0f);
}

void AMercyDeathVolume::HandleDeathBeginOverlap(
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

	if (!IsValidPlayerActor(OtherActor))
	{
		return;
	}

	ProcessDeath(OtherActor);
}

void AMercyDeathVolume::EnableDeathVolume()
{
	bEnabled = true;
}

void AMercyDeathVolume::DisableDeathVolume()
{
	bEnabled = false;
}

void AMercyDeathVolume::ProcessDeath(AActor* VictimActor)
{
	if (!VictimActor)
	{
		return;
	}

	UMercyProtocolComponent* ProtocolComponent = VictimActor->FindComponentByClass<UMercyProtocolComponent>();

	if (ProtocolComponent)
	{
		ProtocolComponent->AddDeath();
		ProtocolComponent->AddMercyProtocolPercent(3.0f);
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySound2D(this, DeathSound);
	}

	ShowDeathText();

	OnDeathVolumeTriggered.Broadcast(VictimActor);

	DebugMessage(TEXT("Death volume triggered"), FColor::Red, 5.0f);

	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindUObject(this, &AMercyDeathVolume::RespawnActor, VictimActor);

	FTimerHandle RespawnTimerHandle;
	GetWorldTimerManager().SetTimer(
		RespawnTimerHandle,
		RespawnDelegate,
		FMath::Max(0.01f, RespawnDelay),
		false
	);
}

void AMercyDeathVolume::RespawnActor(AActor* VictimActor)
{
	if (!VictimActor)
	{
		return;
	}

	UMercyRespawnComponent* RespawnComponent = VictimActor->FindComponentByClass<UMercyRespawnComponent>();

	if (RespawnComponent && RespawnComponent->RespawnOwnerAtCheckpoint())
	{
		return;
	}

	if (FallbackRespawnTarget)
	{
		VictimActor->SetActorTransform(
			FallbackRespawnTarget->GetActorTransform(),
			false,
			nullptr,
			ETeleportType::TeleportPhysics
		);

		DebugMessage(TEXT("Respawned at fallback target"), FColor::Cyan, 4.0f);
	}
}

void AMercyDeathVolume::ShowDeathText()
{
	TArray<AActor*> TextActors;

	if (!SystemTextTag.IsNone())
	{
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), SystemTextTag, TextActors);
	}
	else
	{
		for (AMercySystemTextActor* SystemTextActor : TActorRange<AMercySystemTextActor>(GetWorld()))
		{
			if (SystemTextActor)
			{
				SystemTextActor->ShowTypewriterMessage(DeathMessage, 0.035f, 5.0f);
			}
		}
		return;
	}

	for (AActor* Actor : TextActors)
	{
		AMercySystemTextActor* SystemTextActor = Cast<AMercySystemTextActor>(Actor);

		if (SystemTextActor)
		{
			SystemTextActor->ShowTypewriterMessage(DeathMessage, 0.035f, 5.0f);
		}
	}
}

bool AMercyDeathVolume::IsValidPlayerActor(AActor* OtherActor) const
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

void AMercyDeathVolume::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}