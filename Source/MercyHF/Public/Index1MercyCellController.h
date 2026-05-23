#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Index1MercyCellController.generated.h"

class USoundBase;

UCLASS()
class MERCYHF_API AIndex1MercyCellController : public AActor
{
	GENERATED_BODY()

public:
	AIndex1MercyCellController();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 1")
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 1")
	float InitialDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 1")
	float MessageInterval = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 1")
	float TypewriterInterval = 0.035f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 1")
	FName SystemTextTag = TEXT("SYSTEM_TEXT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 1")
	FName ExitActorsTag = TEXT("INDEX1_EXIT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 1")
	FName ObservationLightTag = TEXT("INDEX1_OBSERVATION_LIGHT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 1")
	USoundBase* RegistrationSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 1")
	USoundBase* ExitRevealSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	TArray<FString> MessageSequence;
	int32 CurrentMessageIndex = INDEX_NONE;

	FTimerHandle InitialTimerHandle;
	FTimerHandle MessageTimerHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Index 1")
	void StartMercyCellSequence();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Index 1")
	void RevealExit();

private:
	void BuildDefaultMessages();
	void ShowNextMessage();
	void ShowSystemText(const FString& Message, float AutoHideAfter = 0.0f);
	void SetActorsHiddenByTag(FName Tag, bool bShouldHide);
	void SetLightIntensityByTag(FName Tag, float NewIntensity);
	void PlaySound2DIfValid(USoundBase* Sound);
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};