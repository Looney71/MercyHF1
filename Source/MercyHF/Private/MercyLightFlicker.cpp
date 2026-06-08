#include "MercyLightFlicker.h"

#include "Components/LightComponent.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AMercyLightFlicker::AMercyLightFlicker()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMercyLightFlicker::BeginPlay()
{
	Super::BeginPlay();

	if (bCacheLightsOnBeginPlay)
	{
		CacheTargetLights();
	}

	if (bStartFlickeringOnBeginPlay)
	{
		StartFlicker();
	}

	DebugMessage(TEXT("MercyLightFlicker ready"), FColor::Cyan, 3.0f);
}

void AMercyLightFlicker::StartFlicker()
{
	if (bIsFlickering)
	{
		return;
	}

	bIsFlickering = true;

	FlickerTick();

	DebugMessage(TEXT("Light flicker started"), FColor::Yellow, 3.0f);
}

void AMercyLightFlicker::StopFlicker()
{
	bIsFlickering = false;
	GetWorldTimerManager().ClearTimer(FlickerTimerHandle);

	if (bRestoreIntensityWhenStopped)
	{
		SetAllLightIntensity(RestoredIntensity);
	}

	DebugMessage(TEXT("Light flicker stopped"), FColor::Green, 3.0f);
}

void AMercyLightFlicker::PulseOnce(float PulseIntensity, float ReturnDelay)
{
	GetWorldTimerManager().ClearTimer(FlickerTimerHandle);

	SetAllLightIntensity(PulseIntensity);

	GetWorldTimerManager().SetTimer(
		FlickerTimerHandle,
		this,
		&AMercyLightFlicker::RestoreAfterPulse,
		FMath::Max(0.01f, ReturnDelay),
		false
	);

	DebugMessage(TEXT("Light pulse"), FColor::Red, 2.0f);
}

void AMercyLightFlicker::SetAllLightIntensity(float NewIntensity)
{
	for (AActor* Actor : TargetLightActors)
	{
		ApplyIntensityToActor(Actor, NewIntensity);
	}
}

void AMercyLightFlicker::CacheTargetLights()
{
	TargetLightActors.Empty();

	for (TActorRange<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		if (ActorMatchesTarget(Actor))
		{
			TargetLightActors.Add(Actor);
		}
	}

	DebugMessage(
		FString::Printf(TEXT("Flicker lights found: %d"), TargetLightActors.Num()),
		FColor::Cyan,
		4.0f
	);
}

void AMercyLightFlicker::FlickerTick()
{
	if (!bIsFlickering)
	{
		return;
	}

	float NewIntensity = FMath::FRandRange(MinIntensity, MaxIntensity);

	if (bSometimesTurnOffCompletely && FMath::FRand() <= FullBlackoutChance)
	{
		NewIntensity = 0.0f;
	}

	SetAllLightIntensity(NewIntensity);

	const float NextInterval = FMath::FRandRange(
		FMath::Min(MinFlickerInterval, MaxFlickerInterval),
		FMath::Max(MinFlickerInterval, MaxFlickerInterval)
	);

	GetWorldTimerManager().SetTimer(
		FlickerTimerHandle,
		this,
		&AMercyLightFlicker::FlickerTick,
		NextInterval,
		false
	);
}

void AMercyLightFlicker::RestoreAfterPulse()
{
	SetAllLightIntensity(RestoredIntensity);

	if (bIsFlickering)
	{
		FlickerTick();
	}
}

void AMercyLightFlicker::ApplyIntensityToActor(AActor* Actor, float NewIntensity)
{
	if (!Actor)
	{
		return;
	}

	ULightComponent* LightComponent = Actor->FindComponentByClass<ULightComponent>();

	if (LightComponent)
	{
		LightComponent->SetIntensity(NewIntensity);
	}
}

bool AMercyLightFlicker::ActorMatchesTarget(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	if (!TargetLightTag.IsNone() && Actor->ActorHasTag(TargetLightTag))
	{
		return true;
	}

	if (Actor->FindComponentByClass<ULightComponent>() == nullptr)
	{
		return false;
	}

	const FString ActorName = Actor->GetName();

	if (!TargetLightNameContains.IsEmpty() && ActorName.Contains(TargetLightNameContains))
	{
		return true;
	}

#if WITH_EDITOR
	const FString ActorEditorLabel = Actor->GetActorLabel();

	if (!TargetLightNameContains.IsEmpty() && ActorEditorLabel.Contains(TargetLightNameContains))
	{
		return true;
	}
#endif

	return false;
}

void AMercyLightFlicker::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}