#include "Index5PersonalIndexController.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "MercySystemTextActor.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AIndex5PersonalIndexController::AIndex5PersonalIndexController()
{
	PrimaryActorTick.bCanEverTick = false;
	BuildDefaultChambers();
}

void AIndex5PersonalIndexController::BeginPlay()
{
	Super::BeginPlay();

	BuildDefaultChambers();

	for (const FMercyPersonalIndexChamber& Chamber : Chambers)
	{
		SetActorsHiddenByTag(Chamber.ActorsToShowTag, true);
	}

	if (bAutoStart)
	{
		GetWorldTimerManager().SetTimer(
			InitialTimerHandle,
			this,
			&AIndex5PersonalIndexController::StartPersonalIndex,
			InitialDelay,
			false
		);
	}

	DebugMessage(TEXT("Index5 Personal Index Controller ready"), FColor::Cyan, 3.0f);
}

void AIndex5PersonalIndexController::BuildDefaultChambers()
{
	if (Chambers.Num() > 0)
	{
		return;
	}

	FMercyPersonalIndexChamber Fear;
	Fear.ChamberId = TEXT("FEAR");
	Fear.ChamberTitle = TEXT("FEAR");
	Fear.SystemMessage = TEXT("FEAR RESPONSE: ACTIVE");
	Fear.ActorsToShowTag = TEXT("INDEX5_FEAR");
	Fear.ChamberDuration = 7.0f;
	Chambers.Add(Fear);

	FMercyPersonalIndexChamber Guilt;
	Guilt.ChamberId = TEXT("GUILT");
	Guilt.ChamberTitle = TEXT("GUILT");
	Guilt.SystemMessage = TEXT("HELP REQUEST DETECTED");
	Guilt.ActorsToShowTag = TEXT("INDEX5_GUILT");
	Guilt.ChamberDuration = 7.0f;
	Chambers.Add(Guilt);

	FMercyPersonalIndexChamber Memory;
	Memory.ChamberId = TEXT("MEMORY");
	Memory.ChamberTitle = TEXT("MEMORY");
	Memory.SystemMessage = TEXT("MEMORY STABILITY: LOW");
	Memory.ActorsToShowTag = TEXT("INDEX5_MEMORY");
	Memory.ChamberDuration = 7.0f;
	Chambers.Add(Memory);

	FMercyPersonalIndexChamber Resistance;
	Resistance.ChamberId = TEXT("RESISTANCE");
	Resistance.ChamberTitle = TEXT("RESISTANCE");
	Resistance.SystemMessage = TEXT("RESISTANCE IS STILL DATA");
	Resistance.ActorsToShowTag = TEXT("INDEX5_RESISTANCE");
	Resistance.ChamberDuration = 7.0f;
	Chambers.Add(Resistance);

	FMercyPersonalIndexChamber Mercy;
	Mercy.ChamberId = TEXT("MERCY");
	Mercy.ChamberTitle = TEXT("MERCY");
	Mercy.SystemMessage = TEXT("MERCY RESPONSE: INCOMPLETE");
	Mercy.ActorsToShowTag = TEXT("INDEX5_MERCY");
	Mercy.ChamberDuration = 7.0f;
	Chambers.Add(Mercy);
}

void AIndex5PersonalIndexController::StartPersonalIndex()
{
	CurrentChamberIndex = 0;
	ShowCurrentChamber();

	DebugMessage(TEXT("INDEX-5 Personal Index started"), FColor::Green, 5.0f);
}

void AIndex5PersonalIndexController::AdvanceChamber()
{
	if (!Chambers.IsValidIndex(CurrentChamberIndex))
	{
		CompletePersonalIndex();
		return;
	}

	const FMercyPersonalIndexChamber& PreviousChamber = Chambers[CurrentChamberIndex];
	SetActorsHiddenByTag(PreviousChamber.ActorsToShowTag, true);
	SetActorsHiddenByTag(PreviousChamber.ActorsToHideTag, false);

	++CurrentChamberIndex;

	ShowCurrentChamber();
}

void AIndex5PersonalIndexController::ShowCurrentChamber()
{
	if (!Chambers.IsValidIndex(CurrentChamberIndex))
	{
		CompletePersonalIndex();
		return;
	}

	const FMercyPersonalIndexChamber& CurrentChamber = Chambers[CurrentChamberIndex];

	SetActorsHiddenByTag(CurrentChamber.ActorsToShowTag, false);
	SetActorsHiddenByTag(CurrentChamber.ActorsToHideTag, true);

	PlaySound2DIfValid(ChamberAdvanceSound);

	const FString FullMessage = FString::Printf(
		TEXT("%s\n%s"),
		*CurrentChamber.ChamberTitle,
		*CurrentChamber.SystemMessage
	);

	ShowSystemText(FullMessage);

	GetWorldTimerManager().SetTimer(
		ChamberTimerHandle,
		this,
		&AIndex5PersonalIndexController::AdvanceChamber,
		FMath::Max(0.1f, CurrentChamber.ChamberDuration),
		false
	);

	DebugMessage(
		FString::Printf(TEXT("INDEX-5 chamber: %s"), *CurrentChamber.ChamberId.ToString()),
		FColor::Yellow,
		4.0f
	);
}

void AIndex5PersonalIndexController::CompletePersonalIndex()
{
	GetWorldTimerManager().ClearTimer(ChamberTimerHandle);

	PlaySound2DIfValid(CompletionSound);

	ShowSystemText(TEXT("PERSONAL INDEX CREATED\nGROUP CLASSIFICATION: UNSTABLE\nMERCY PROTOCOL: 31%"), 10.0f);

	DebugMessage(TEXT("INDEX-5 completed"), FColor::Green, 8.0f);
}

void AIndex5PersonalIndexController::ShowSystemText(const FString& Message, float AutoHideAfter)
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
			TextActor->ShowTypewriterMessage(Message, 0.035f, AutoHideAfter);
		}
	}
}

void AIndex5PersonalIndexController::SetActorsHiddenByTag(FName Tag, bool bShouldHide)
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

void AIndex5PersonalIndexController::PlaySound2DIfValid(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound);
	}
}

void AIndex5PersonalIndexController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}