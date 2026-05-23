#include "MercyLevelTransition.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AMercyLevelTransition::AMercyLevelTransition()
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

void AMercyLevelTransition::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->SetBoxExtent(TriggerBoxExtent);
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMercyLevelTransition::HandleTriggerBeginOverlap);
	}

	DebugMessage(TEXT("MercyLevelTransition ready"), FColor::Cyan, 3.0f);
}

void AMercyLevelTransition::HandleTriggerBeginOverlap(
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

	StartTransition();
}

void AMercyLevelTransition::StartTransition()
{
	if (!bEnabled)
	{
		return;
	}

	if (bTriggerOnlyOnce && bHasTriggered)
	{
		return;
	}

	if (TargetLevelName.IsNone())
	{
		DebugMessage(TEXT("Transition failed: TargetLevelName is empty"), FColor::Red, 5.0f);
		return;
	}

	bHasTriggered = true;
	OnTransitionStarted.Broadcast(TargetLevelName);

	if (TransitionSound)
	{
		UGameplayStatics::PlaySound2D(this, TransitionSound);
	}

	DebugMessage(FString::Printf(TEXT("Transition started: %s"), *TargetLevelName.ToString()), FColor::Green, 4.0f);

	if (TransitionDelay > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			TransitionTimerHandle,
			this,
			&AMercyLevelTransition::ExecuteLevelOpen,
			TransitionDelay,
			false
		);
	}
	else
	{
		ExecuteLevelOpen();
	}
}

void AMercyLevelTransition::SetTargetLevel(FName NewTargetLevelName)
{
	TargetLevelName = NewTargetLevelName;
}

void AMercyLevelTransition::EnableTransition()
{
	bEnabled = true;
}

void AMercyLevelTransition::DisableTransition()
{
	bEnabled = false;
}

void AMercyLevelTransition::ExecuteLevelOpen()
{
	UGameplayStatics::OpenLevel(this, TargetLevelName);
}

bool AMercyLevelTransition::IsValidPlayerActor(AActor* OtherActor) const
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

void AMercyLevelTransition::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}