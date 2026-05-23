#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Index5PersonalIndexController.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FMercyPersonalIndexChamber
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Index 5")
	FName ChamberId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Index 5")
	FString ChamberTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Index 5")
	FString SystemMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Index 5")
	FName ActorsToShowTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Index 5")
	FName ActorsToHideTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Index 5")
	float ChamberDuration = 7.0f;
};

UCLASS()
class MERCYHF_API AIndex5PersonalIndexController : public AActor
{
	GENERATED_BODY()

public:
	AIndex5PersonalIndexController();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 5")
	bool bAutoStart = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 5")
	float InitialDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 5")
	FName SystemTextTag = TEXT("SYSTEM_TEXT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 5")
	TArray<FMercyPersonalIndexChamber> Chambers;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 5")
	USoundBase* ChamberAdvanceSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 5")
	USoundBase* CompletionSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	int32 CurrentChamberIndex = INDEX_NONE;

	FTimerHandle InitialTimerHandle;
	FTimerHandle ChamberTimerHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Index 5")
	void StartPersonalIndex();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Index 5")
	void AdvanceChamber();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Index 5")
	void CompletePersonalIndex();

private:
	void BuildDefaultChambers();
	void ShowCurrentChamber();
	void ShowSystemText(const FString& Message, float AutoHideAfter = 0.0f);
	void SetActorsHiddenByTag(FName Tag, bool bShouldHide);
	void PlaySound2DIfValid(USoundBase* Sound);
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};