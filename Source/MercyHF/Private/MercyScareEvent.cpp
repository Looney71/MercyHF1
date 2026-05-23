#include "MercyScareEvent.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AMercyScareEvent::AMercyScareEvent()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneRoot);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMercyScareEvent::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->SetBoxExtent(TriggerBoxExtent);
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMercyScareEvent::HandleTriggerBeginOverlap);
	}

	SetActorsHidden(ActorsToShow, true);
	SetActorsHiddenByTag(ActorsToShowTag, true);

	DebugMessage(TEXT("MercyScareEvent ready"), FColor::Cyan, 3.0f);
}

void AMercyScareEvent::HandleTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!bEnabled || !bTriggerOnPlayerOverlap)
	{
		return;
	}

	if (!IsValidPlayerActor(OtherActor))
	{
		return;
	}

	ActivateScare();
}

void AMercyScareEvent::ActivateScare()
{
	if (!bEnabled)
	{
		return;
	}

	if (bTriggerOnlyOnce && bHasTriggered)
	{
		return;
	}

	bHasTriggered = true;

	if (ActivationDelay > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			ActivationTimerHandle,
			this,
			&AMercyScareEvent::ExecuteScare,
			ActivationDelay,
			false
		);
	}
	else
	{
		ExecuteScare();
	}
}

void AMercyScareEvent::ResetScare()
{
	bHasTriggered = false;
	GetWorldTimerManager().ClearTimer(ActivationTimerHandle);
	GetWorldTimerManager().ClearTimer(AutoHideTimerHandle);

	SetActorsHidden(ActorsToShow, true);
	SetActorsHiddenByTag(ActorsToShowTag, true);

	DebugMessage(TEXT("MercyScareEvent reset"), FColor::Cyan, 3.0f);
}

void AMercyScareEvent::EnableScare()
{
	bEnabled = true;
	DebugMessage(TEXT("MercyScareEvent enabled"), FColor::Green, 3.0f);
}

void AMercyScareEvent::DisableScare()
{
	bEnabled = false;
	DebugMessage(TEXT("MercyScareEvent disabled"), FColor::Yellow, 3.0f);
}

bool AMercyScareEvent::HasTriggered() const
{
	return bHasTriggered;
}

void AMercyScareEvent::ExecuteScare()
{
	if (ScareSound)
	{
		if (bPlaySound2D)
		{
			UGameplayStatics::PlaySound2D(this, ScareSound, VolumeMultiplier);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(this, ScareSound, GetActorLocation(), VolumeMultiplier);
		}
	}

	SetActorsHidden(ActorsToShow, false);
	SetActorsHiddenByTag(ActorsToShowTag, false);

	SetActorsHidden(ActorsToHide, true);
	SetActorsHiddenByTag(ActorsToHideTag, true);

	if (bEnableCollisionOnShownActors)
	{
		SetActorListCollision(ActorsToShow, true);
		SetActorCollisionByTag(ActorsToShowTag, true);
	}

	if (bDisableCollisionOnHiddenActors)
	{
		SetActorListCollision(ActorsToHide, false);
		SetActorCollisionByTag(ActorsToHideTag, false);
	}

	OnScareActivated.Broadcast();

	if (bAutoHideShownActors)
	{
		GetWorldTimerManager().SetTimer(
			AutoHideTimerHandle,
			this,
			&AMercyScareEvent::AutoHideShownActors,
			AutoHideDelay,
			false
		);
	}

	DebugMessage(TEXT("Mercy scare activated"), FColor::Red, 4.0f);
}

void AMercyScareEvent::AutoHideShownActors()
{
	SetActorsHidden(ActorsToShow, true);
	SetActorsHiddenByTag(ActorsToShowTag, true);

	if (bDisableCollisionOnHiddenActors)
	{
		SetActorListCollision(ActorsToShow, false);
		SetActorCollisionByTag(ActorsToShowTag, false);
	}

	DebugMessage(TEXT("Mercy scare auto-hidden"), FColor::Yellow, 3.0f);
}

void AMercyScareEvent::SetActorsHidden(const TArray<AActor*>& Actors, bool bShouldHide)
{
	for (AActor* Actor : Actors)
	{
		if (Actor)
		{
			Actor->SetActorHiddenInGame(bShouldHide);
		}
	}
}

void AMercyScareEvent::SetActorsHiddenByTag(FName Tag, bool bShouldHide)
{
	if (Tag.IsNone())
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	SetActorsHidden(FoundActors, bShouldHide);
}

void AMercyScareEvent::SetActorListCollision(const TArray<AActor*>& Actors, bool bEnableCollision)
{
	for (AActor* Actor : Actors)
	{
		if (Actor)
		{
			Actor->SetActorEnableCollision(bEnableCollision);
		}
	}
}

void AMercyScareEvent::SetActorCollisionByTag(FName Tag, bool bEnableCollision)
{
	if (Tag.IsNone())
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	SetActorListCollision(FoundActors, bEnableCollision);
}

bool AMercyScareEvent::IsValidPlayerActor(AActor* OtherActor) const
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

void AMercyScareEvent::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}