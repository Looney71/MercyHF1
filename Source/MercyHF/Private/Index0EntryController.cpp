#include "Index0EntryController.h"

#include "Components/LightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AIndex0EntryController::AIndex0EntryController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AIndex0EntryController::BeginPlay()
{
	Super::BeginPlay();

	DebugMessage(TEXT("Index0 Entry Controller started"), FColor::Green, 4.0f);

	CacheEntryActors();

	DebugMessage(FString::Printf(TEXT("Main lights found: %d"), MainLightActors.Num()), FColor::Cyan, 5.0f);
	DebugMessage(FString::Printf(TEXT("Red lights found: %d"), RedLightActors.Num()), FColor::Cyan, 5.0f);
	DebugMessage(FString::Printf(TEXT("Warning texts found: %d"), WarningTextActors.Num()), FColor::Cyan, 5.0f);
	DebugMessage(FString::Printf(TEXT("Path reveal actors found: %d"), PathRevealActors.Num()), FColor::Cyan, 5.0f);

	SetActorsHidden(WarningTextActors, true);
	SetActorsHidden(PathRevealActors, true);

	SetActorsHidden(MainLightActors, false);
	SetActorsHidden(RedLightActors, false);

	SetLightIntensity(RedLightActors, RedLightIdleIntensity);
	SetTextForActors(WarningTextActors, TEXT("DO NOT FOLLOW VOICES"));

	GetWorldTimerManager().SetTimer(
		LightsOffTimerHandle,
		this,
		&AIndex0EntryController::StartBlackout,
		LightsOffDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		WarningTextTimerHandle,
		this,
		&AIndex0EntryController::ShowWarningText,
		WarningTextDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		HelpVoiceTimerHandle,
		this,
		&AIndex0EntryController::PlayHelpVoice,
		HelpVoiceDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		PathRevealTimerHandle,
		this,
		&AIndex0EntryController::RevealPathForward,
		PathRevealDelay,
		false
	);
}

void AIndex0EntryController::CacheEntryActors()
{
	MainLightActors.Empty();
	RedLightActors.Empty();
	WarningTextActors.Empty();
	PathRevealActors.Empty();

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (!Actor)
		{
			continue;
		}

		if (ActorMatches(Actor, MainLightTag, TEXT("L_Entry_Main")))
		{
			MainLightActors.Add(Actor);
		}

		if (ActorMatches(Actor, RedLightTag, TEXT("L_Entry_Red")))
		{
			RedLightActors.Add(Actor);
		}

		if (ActorMatches(Actor, WarningTextTag, TEXT("TXT_DoNotFollowVoices")))
		{
			WarningTextActors.Add(Actor);
		}

		if (ActorMatches(Actor, PathRevealTag, TEXT("Entry_Path")))
		{
			PathRevealActors.Add(Actor);
		}
	}
}

bool AIndex0EntryController::ActorMatches(AActor* Actor, FName RequiredTag, const FString& NameContains) const
{
	if (!Actor)
	{
		return false;
	}

	if (!RequiredTag.IsNone() && Actor->ActorHasTag(RequiredTag))
	{
		return true;
	}

	const FString ActorName = Actor->GetName();

	if (ActorName.Contains(NameContains))
	{
		return true;
	}

#if WITH_EDITOR
	const FString ActorEditorLabel = Actor->GetActorLabel();

	if (ActorEditorLabel.Contains(NameContains))
	{
		return true;
	}
#endif

	return false;
}

void AIndex0EntryController::StartBlackout()
{
	DebugMessage(TEXT("ENTRY: blackout started"), FColor::Red, 4.0f);

	SetActorsHidden(MainLightActors, true);
	SetLightIntensity(RedLightActors, RedLightBlackoutIntensity);
}

void AIndex0EntryController::ShowWarningText()
{
	DebugMessage(TEXT("ENTRY: warning text shown"), FColor::Yellow, 4.0f);

	SetTextForActors(WarningTextActors, TEXT("DO NOT FOLLOW VOICES"));
	SetActorsHidden(WarningTextActors, false);
}

void AIndex0EntryController::PlayHelpVoice()
{
	USoundBase* SoundToPlay = HelpVoiceSound;

	if (!SoundToPlay && !HelpSoundAssetPath.IsEmpty())
	{
		SoundToPlay = Cast<USoundBase>(
			StaticLoadObject(USoundBase::StaticClass(), nullptr, *HelpSoundAssetPath)
		);
	}

	if (SoundToPlay)
	{
		DebugMessage(TEXT("ENTRY: help voice played"), FColor::Purple, 4.0f);
		UGameplayStatics::PlaySound2D(this, SoundToPlay);
	}
	else
	{
		DebugMessage(TEXT("ENTRY: help voice sound not found yet"), FColor::Orange, 5.0f);
	}
}

void AIndex0EntryController::RevealPathForward()
{
	DebugMessage(TEXT("ENTRY: path forward revealed"), FColor::Green, 4.0f);

	SetActorsHidden(PathRevealActors, false);
	SetTextForActors(WarningTextActors, TEXT("VOLUNTARY ENTRY CONFIRMED"));
}

void AIndex0EntryController::SetActorsHidden(const TArray<AActor*>& Actors, bool bShouldHide)
{
	for (AActor* Actor : Actors)
	{
		if (Actor)
		{
			Actor->SetActorHiddenInGame(bShouldHide);
			Actor->SetActorEnableCollision(!bShouldHide);

			if (ULightComponent* LightComp = Actor->FindComponentByClass<ULightComponent>())
			{
				if (bShouldHide)
				{
					LightComp->SetVisibility(false, true);
					LightComp->SetIntensity(0.0f);
				}
				else
				{
					LightComp->SetVisibility(true, true);
				}
			}
		}
	}
}

void AIndex0EntryController::SetLightIntensity(const TArray<AActor*>& Actors, float NewIntensity)
{
	for (AActor* Actor : Actors)
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

void AIndex0EntryController::SetTextForActors(const TArray<AActor*>& Actors, const FString& NewText)
{
	for (AActor* Actor : Actors)
	{
		if (!Actor)
		{
			continue;
		}

		UTextRenderComponent* TextComponent = Actor->FindComponentByClass<UTextRenderComponent>();

		if (TextComponent)
		{
			TextComponent->SetText(FText::FromString(NewText));
		}
	}
}

void AIndex0EntryController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}