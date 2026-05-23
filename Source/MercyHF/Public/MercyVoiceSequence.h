#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyVoiceSequence.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FMercyVoiceLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	float DelayBeforeLine = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	FString Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	USoundBase* VoiceSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	FString SoundAssetPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	bool bPlayAs2DSound = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	bool bUseTypewriterText = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	float TypewriterInterval = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	float AutoHideTextAfter = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Voice")
	FName TargetSystemTextTag = NAME_None;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMercyVoiceSequenceFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyVoiceLinePlayed, int32, LineIndex);

UCLASS()
class MERCYHF_API AMercyVoiceSequence : public AActor
{
	GENERATED_BODY()

public:
	AMercyVoiceSequence();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Voice")
	bool bAutoPlayOnBeginPlay = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Voice")
	bool bLoopSequence = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Voice")
	TArray<FMercyVoiceLine> VoiceLines;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	int32 CurrentLineIndex = INDEX_NONE;
	bool bSequencePlaying = false;

	FTimerHandle NextLineTimerHandle;

public:
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Voice")
	FMercyVoiceSequenceFinished OnVoiceSequenceFinished;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Voice")
	FMercyVoiceLinePlayed OnVoiceLinePlayed;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Voice")
	void PlaySequence();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Voice")
	void StopSequence();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Voice")
	void RestartSequence();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Voice")
	void AddVoiceLine(const FMercyVoiceLine& NewLine);

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Voice")
	bool IsSequencePlaying() const;

private:
	void ScheduleNextLine();
	void PlayCurrentLine();
	void AdvanceLine();

	USoundBase* ResolveSound(const FMercyVoiceLine& Line);
	void PlayLineSound(const FMercyVoiceLine& Line, USoundBase* ResolvedSound);
	void ShowLineText(const FMercyVoiceLine& Line);

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};