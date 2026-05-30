#include "MercyVoiceSequence.h"

#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "MercySystemTextActor.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AMercyVoiceSequence::AMercyVoiceSequence()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMercyVoiceSequence::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoPlayOnBeginPlay)
	{
		PlaySequence();
	}

	DebugMessage(TEXT("MercyVoiceSequence ready"), FColor::Cyan, 3.0f);
}

void AMercyVoiceSequence::PlaySequence()
{
	if (VoiceLines.Num() <= 0)
	{
		DebugMessage(TEXT("MercyVoiceSequence: no voice lines"), FColor::Orange, 4.0f);
		return;
	}

	bSequencePlaying = true;
	CurrentLineIndex = 0;

	ScheduleNextLine();

	DebugMessage(TEXT("Voice sequence started"), FColor::Green, 3.0f);
}

void AMercyVoiceSequence::StopSequence()
{
	bSequencePlaying = false;
	CurrentLineIndex = INDEX_NONE;

	GetWorldTimerManager().ClearTimer(NextLineTimerHandle);

	DebugMessage(TEXT("Voice sequence stopped"), FColor::Yellow, 3.0f);
}

void AMercyVoiceSequence::RestartSequence()
{
	StopSequence();
	PlaySequence();
}

void AMercyVoiceSequence::AddVoiceLine(const FMercyVoiceLine& NewLine)
{
	VoiceLines.Add(NewLine);
}

bool AMercyVoiceSequence::IsSequencePlaying() const
{
	return bSequencePlaying;
}

void AMercyVoiceSequence::ScheduleNextLine()
{
	if (!bSequencePlaying || !VoiceLines.IsValidIndex(CurrentLineIndex))
	{
		return;
	}

	const FMercyVoiceLine& CurrentLine = VoiceLines[CurrentLineIndex];
	const float SafeDelay = FMath::Max(0.0f, CurrentLine.DelayBeforeLine);

	GetWorldTimerManager().SetTimer(
		NextLineTimerHandle,
		this,
		&AMercyVoiceSequence::PlayCurrentLine,
		SafeDelay,
		false
	);
}

void AMercyVoiceSequence::PlayCurrentLine()
{
	if (!bSequencePlaying || !VoiceLines.IsValidIndex(CurrentLineIndex))
	{
		return;
	}

	const FMercyVoiceLine& CurrentLine = VoiceLines[CurrentLineIndex];

	USoundBase* ResolvedSound = ResolveSound(CurrentLine);

	if (ResolvedSound)
	{
		PlayLineSound(CurrentLine, ResolvedSound);
	}

	if (!CurrentLine.Message.IsEmpty())
	{
		ShowLineText(CurrentLine);
	}

	OnVoiceLinePlayed.Broadcast(CurrentLineIndex);

	DebugMessage(
		FString::Printf(TEXT("Voice line played: %d"), CurrentLineIndex),
		FColor::Purple,
		3.0f
	);

	AdvanceLine();
}

void AMercyVoiceSequence::AdvanceLine()
{
	++CurrentLineIndex;

	if (VoiceLines.IsValidIndex(CurrentLineIndex))
	{
		ScheduleNextLine();
		return;
	}

	if (bLoopSequence)
	{
		CurrentLineIndex = 0;
		ScheduleNextLine();
		return;
	}

	bSequencePlaying = false;
	CurrentLineIndex = INDEX_NONE;

	OnVoiceSequenceFinished.Broadcast();

	DebugMessage(TEXT("Voice sequence finished"), FColor::Green, 4.0f);
}

USoundBase* AMercyVoiceSequence::ResolveSound(const FMercyVoiceLine& Line)
{
	if (Line.VoiceSound)
	{
		return Line.VoiceSound;
	}

	if (Line.SoundAssetPath.IsEmpty())
	{
		return nullptr;
	}

	return Cast<USoundBase>(
		StaticLoadObject(USoundBase::StaticClass(), nullptr, *Line.SoundAssetPath)
	);
}

void AMercyVoiceSequence::PlayLineSound(const FMercyVoiceLine& Line, USoundBase* ResolvedSound)
{
	if (!ResolvedSound)
	{
		return;
	}

	if (Line.bPlayAs2DSound)
	{
		UGameplayStatics::PlaySound2D(
			this,
			ResolvedSound,
			Line.VolumeMultiplier,
			Line.PitchMultiplier
		);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ResolvedSound,
			GetActorLocation(),
			Line.VolumeMultiplier,
			Line.PitchMultiplier
		);
	}
}

void AMercyVoiceSequence::ShowLineText(const FMercyVoiceLine& Line)
{
	if (!Line.TargetSystemTextTag.IsNone())
	{
		TArray<AActor*> TextActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), Line.TargetSystemTextTag, TextActors);

		for (AActor* Actor : TextActors)
		{
			AMercySystemTextActor* SystemTextActor = Cast<AMercySystemTextActor>(Actor);

			if (!SystemTextActor)
			{
				continue;
			}

			if (Line.bUseTypewriterText)
			{
				SystemTextActor->ShowTypewriterMessage(
					Line.Message,
					Line.TypewriterInterval,
					Line.AutoHideTextAfter
				);
			}
			else
			{
				SystemTextActor->ShowInstantMessage(Line.Message, Line.AutoHideTextAfter);
			}
		}
	}
	else
	{
		for (AMercySystemTextActor* SystemTextActor : TActorRange<AMercySystemTextActor>(GetWorld()))
		{
			if (!SystemTextActor)
			{
				continue;
			}

			if (Line.bUseTypewriterText)
			{
				SystemTextActor->ShowTypewriterMessage(
					Line.Message,
					Line.TypewriterInterval,
					Line.AutoHideTextAfter
				);
			}
			else
			{
				SystemTextActor->ShowInstantMessage(
					Line.Message,
					Line.AutoHideTextAfter
				);
			}
		}
	}
}

void AMercyVoiceSequence::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}