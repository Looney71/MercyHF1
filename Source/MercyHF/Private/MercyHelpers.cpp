#include "MercyHelpers.h"

#include "Components/LightComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MercySystemTextActor.h"
#include "Sound/SoundBase.h"

void UMercyHelpers::SetActorsHiddenByTag(const UObject* WorldContextObject, FName Tag, bool bHidden, bool bUpdateCollision)
{
	if (Tag.IsNone() || !WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;

	if (!World)
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(World, Tag, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (!Actor)
		{
			continue;
		}

		Actor->SetActorHiddenInGame(bHidden);

		if (bUpdateCollision)
		{
			Actor->SetActorEnableCollision(!bHidden);
		}
	}
}

void UMercyHelpers::SetLightIntensityByTag(const UObject* WorldContextObject, FName Tag, float NewIntensity)
{
	if (Tag.IsNone() || !WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;

	if (!World)
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(World, Tag, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (!Actor)
		{
			continue;
		}

		ULightComponent* LightComponent = Actor->FindComponentByClass<ULightComponent>();

		if (LightComponent)
		{
			LightComponent->SetIntensity(NewIntensity);
		}
	}
}

void UMercyHelpers::ShowSystemTextByTag(
	const UObject* WorldContextObject,
	FName Tag,
	const FString& Message,
	float CharacterIntervalSeconds,
	float AutoHideAfterSeconds
)
{
	if (!WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;

	if (!World)
	{
		return;
	}

	if (!Tag.IsNone())
	{
		TArray<AActor*> TextActors;
		UGameplayStatics::GetAllActorsWithTag(World, Tag, TextActors);

		for (AActor* Actor : TextActors)
		{
			AMercySystemTextActor* TextActor = Cast<AMercySystemTextActor>(Actor);

			if (TextActor)
			{
				TextActor->ShowTypewriterMessage(Message, CharacterIntervalSeconds, AutoHideAfterSeconds);
			}
		}
	}
	else
	{
		for (AMercySystemTextActor* TextActor : TActorRange<AMercySystemTextActor>(World))
		{
			if (TextActor)
			{
				TextActor->ShowTypewriterMessage(Message, CharacterIntervalSeconds, AutoHideAfterSeconds);
			}
		}
	}
}

void UMercyHelpers::PlaySound2DIfValid(const UObject* WorldContextObject, USoundBase* Sound)
{
	if (!Sound || !WorldContextObject)
	{
		return;
	}

	UGameplayStatics::PlaySound2D(WorldContextObject, Sound);
}

bool UMercyHelpers::IsValidPlayerActor(AActor* OtherActor)
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

void UMercyHelpers::DebugMessage(bool bShowOnScreen, const FString& Message, const FColor& Color, float Duration)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowOnScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}
