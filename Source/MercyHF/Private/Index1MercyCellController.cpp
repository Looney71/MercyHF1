#include "Index1MercyCellController.h"

#include "Engine/Engine.h"
#include "MercyHelpers.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AIndex1MercyCellController::AIndex1MercyCellController()
{
	PrimaryActorTick.bCanEverTick = false;
	BuildDefaultMessages();
}

void AIndex1MercyCellController::BeginPlay()
{
	Super::BeginPlay();

	BuildDefaultMessages();

	UMercyHelpers::SetActorsHiddenByTag(this, ExitActorsTag, true);
	UMercyHelpers::SetLightIntensityByTag(this, ObservationLightTag, 350.0f);

	if (bAutoStart)
	{
		GetWorldTimerManager().SetTimer(
			InitialTimerHandle,
			this,
			&AIndex1MercyCellController::StartMercyCellSequence,
			InitialDelay,
			false
		);
	}

	DebugMessage(TEXT("Index1 Mercy Cell Controller ready"), FColor::Cyan, 3.0f);
}

void AIndex1MercyCellController::BuildDefaultMessages()
{
	if (MessageSequence.Num() > 0)
	{
		return;
	}

	MessageSequence.Add(TEXT("PERSONAL INDEX ACTIVE"));
	MessageSequence.Add(TEXT("SUBJECT: UNREGISTERED"));
	MessageSequence.Add(TEXT("PLEASE REMAIN STILL"));
	MessageSequence.Add(TEXT("MOVEMENT REGISTERED"));
	MessageSequence.Add(TEXT("COMPLIANCE: LOW"));
	MessageSequence.Add(TEXT("REGISTRATION INCOMPLETE"));
	MessageSequence.Add(TEXT("ACCESS TO INDEX-2 GRANTED"));
	MessageSequence.Add(TEXT("THIS IS NOT AN EXIT"));
}

void AIndex1MercyCellController::StartMercyCellSequence()
{
	CurrentMessageIndex = 0;

	PlaySound2DIfValid(RegistrationSound);
	ShowNextMessage();

	DebugMessage(TEXT("INDEX-1 sequence started"), FColor::Green, 4.0f);
}

void AIndex1MercyCellController::ShowNextMessage()
{
	if (!MessageSequence.IsValidIndex(CurrentMessageIndex))
	{
		RevealExit();
		return;
	}

	ShowSystemText(MessageSequence[CurrentMessageIndex]);
	++CurrentMessageIndex;

	GetWorldTimerManager().SetTimer(
		MessageTimerHandle,
		this,
		&AIndex1MercyCellController::ShowNextMessage,
		MessageInterval,
		false
	);
}

void AIndex1MercyCellController::RevealExit()
{
	GetWorldTimerManager().ClearTimer(MessageTimerHandle);

	UMercyHelpers::SetActorsHiddenByTag(this, ExitActorsTag, false);
	UMercyHelpers::SetLightIntensityByTag(this, ObservationLightTag, 900.0f);

	PlaySound2DIfValid(ExitRevealSound);
	ShowSystemText(TEXT("ACCESS CONFIRMED"), 5.0f);

	DebugMessage(TEXT("INDEX-1 exit revealed"), FColor::Green, 5.0f);
}

void AIndex1MercyCellController::ShowSystemText(const FString& Message, float AutoHideAfter)
{
	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, Message, TypewriterInterval, AutoHideAfter);
}

void AIndex1MercyCellController::SetActorsHiddenByTag(FName Tag, bool bShouldHide)
{
	UMercyHelpers::SetActorsHiddenByTag(this, Tag, bShouldHide);
}

void AIndex1MercyCellController::SetLightIntensityByTag(FName Tag, float NewIntensity)
{
	UMercyHelpers::SetLightIntensityByTag(this, Tag, NewIntensity);
}

void AIndex1MercyCellController::PlaySound2DIfValid(USoundBase* Sound)
{
	UMercyHelpers::PlaySound2DIfValid(this, Sound);
}

void AIndex1MercyCellController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UMercyHelpers::DebugMessage(bShowDebugMessages, Message, Color, Duration);
}
