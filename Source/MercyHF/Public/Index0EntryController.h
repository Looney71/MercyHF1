#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Index0EntryController.generated.h"

class USoundBase;

UCLASS()
class MERCYHF_API AIndex0EntryController : public AActor
{
	GENERATED_BODY()

public:
	AIndex0EntryController();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float LightsOffDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float WarningTextDelay = 2.35f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float HelpVoiceDelay = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float PathRevealDelay = 8.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Lighting")
	float RedLightIdleIntensity = 80.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Lighting")
	float RedLightBlackoutIntensity = 450.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* HelpVoiceSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	FString HelpSoundAssetPath = TEXT("/Game/MercyMaze/Audio/SFX_Help.SFX_Help");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName MainLightTag = TEXT("ENTRY_MAIN_LIGHT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName RedLightTag = TEXT("ENTRY_RED_LIGHT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName WarningTextTag = TEXT("ENTRY_WARNING_TEXT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName PathRevealTag = TEXT("ENTRY_PATH_REVEAL");

	UPROPERTY()
	TArray<AActor*> MainLightActors;

	UPROPERTY()
	TArray<AActor*> RedLightActors;

	UPROPERTY()
	TArray<AActor*> WarningTextActors;

	UPROPERTY()
	TArray<AActor*> PathRevealActors;

	FTimerHandle LightsOffTimerHandle;
	FTimerHandle WarningTextTimerHandle;
	FTimerHandle HelpVoiceTimerHandle;
	FTimerHandle PathRevealTimerHandle;

	void CacheEntryActors();

	void StartBlackout();
	void ShowWarningText();
	void PlayHelpVoice();
	void RevealPathForward();

	bool ActorMatches(AActor* Actor, FName RequiredTag, const FString& NameContains) const;

	void SetActorsHidden(const TArray<AActor*>& Actors, bool bShouldHide);
	void SetLightIntensity(const TArray<AActor*>& Actors, float NewIntensity);
	void SetTextForActors(const TArray<AActor*>& Actors, const FString& NewText);

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};