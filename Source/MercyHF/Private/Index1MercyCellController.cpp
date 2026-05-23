#include "Index1MercyCellController.h"

#include "Components/LightComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "MercySystemTextActor.h"
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

	SetActorsHiddenByTag(ExitActorsTag, true);
	SetLightIntensityByTag(ObservationLightTag, 350.0f);

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

	SetActorsHiddenByTag(ExitActorsTag, false);
	SetLightIntensityByTag(ObservationLightTag, 900.0f);

	PlaySound2DIfValid(ExitRevealSound);
	ShowSystemText(TEXT("ACCESS CONFIRMED"), 5.0f);

	DebugMessage(TEXT("INDEX-1 exit revealed"), FColor::Green, 5.0f);
}

void AIndex1MercyCellController::ShowSystemText(const FString& Message, float AutoHideAfter)
{
	TArray<AActor*> TextActors;

	if (!SystemTextTag.IsNone())
	{
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), SystemTextTag, TextActors);
	}
	else
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMercySystemTextActor::StaticClass(), TextActors);
	}

	for (AActor* Actor : TextActors)
	{
		AMercySystemTextActor* TextActor = Cast<AMercySystemTextActor>(Actor);

		if (TextActor)
		{
			TextActor->ShowTypewriterMessage(Message, TypewriterInterval, AutoHideAfter);
		}
	}
}

void AIndex1MercyCellController::SetActorsHiddenByTag(FName Tag, bool bShouldHide)
{
	if (Tag.IsNone())
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (Actor)
		{
			Actor->SetActorHiddenInGame(bShouldHide);
			Actor->SetActorEnableCollision(!bShouldHide);
		}
	}
}

void AIndex1MercyCellController::SetLightIntensityByTag(FName Tag, float NewIntensity)
{
	if (Tag.IsNone())
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

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

void AIndex1MercyCellController::PlaySound2DIfValid(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound);
	}
}

void AIndex1MercyCellController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}