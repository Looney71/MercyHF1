#include "Index5PersonalIndexController.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "MercyHelpers.h"
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
		UMercyHelpers::SetActorsHiddenByTag(this, Chamber.ActorsToShowTag, true);
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
	UMercyHelpers::SetActorsHiddenByTag(this, PreviousChamber.ActorsToShowTag, true);
	UMercyHelpers::SetActorsHiddenByTag(this, PreviousChamber.ActorsToHideTag, false);

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

	UMercyHelpers::SetActorsHiddenByTag(this, CurrentChamber.ActorsToShowTag, false);
	UMercyHelpers::SetActorsHiddenByTag(this, CurrentChamber.ActorsToHideTag, true);

	UMercyHelpers::PlaySound2DIfValid(this, ChamberAdvanceSound);

	const FString FullMessage = FString::Printf(
		TEXT("%s\n%s"),
		*CurrentChamber.ChamberTitle,
		*CurrentChamber.SystemMessage
	);

	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, FullMessage);

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

	UMercyHelpers::PlaySound2DIfValid(this, CompletionSound);

	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, TEXT("PERSONAL INDEX CREATED\nGROUP CLASSIFICATION: UNSTABLE\nMERCY PROTOCOL: 31%"), 0.035f, 10.0f);

	DebugMessage(TEXT("INDEX-5 completed"), FColor::Green, 8.0f);
}

void AIndex5PersonalIndexController::ShowSystemText(const FString& Message, float AutoHideAfter)
{
	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, Message, 0.035f, AutoHideAfter);
}

void AIndex5PersonalIndexController::SetActorsHiddenByTag(FName Tag, bool bShouldHide)
{
	UMercyHelpers::SetActorsHiddenByTag(this, Tag, bShouldHide);
}

void AIndex5PersonalIndexController::PlaySound2DIfValid(USoundBase* Sound)
{
	UMercyHelpers::PlaySound2DIfValid(this, Sound);
}

void AIndex5PersonalIndexController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UMercyHelpers::DebugMessage(bShowDebugMessages, Message, Color, Duration);
}