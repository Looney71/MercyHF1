#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MercySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FMercySavedIndexData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	FName IndexName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	bool bCompleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	float BestCompletionTimeSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	int32 DeathsInIndex = 0;
};

UCLASS()
class MERCYHF_API UMercySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMercySaveGame();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	FString SaveSlotName = TEXT("MercyMaze_Save_01");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	int32 UserIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	FName LastUnlockedIndex = TEXT("INDEX_0_ENTRY");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	FName LastPlayedIndex = TEXT("INDEX_0_ENTRY");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	float MercyProtocolPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	float FearPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	float MercyResponsePercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	float ResistancePercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	int32 TotalDeathCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	TArray<FMercySavedIndexData> SavedIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	TArray<FName> UnlockedIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	TArray<FName> CompletedIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mercy Maze|Save")
	FString LastSaveUtcTime;

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Save")
	void MarkIndexUnlocked(FName IndexName);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Save")
	void MarkIndexCompleted(FName IndexName, float CompletionTimeSeconds, int32 DeathsInIndex);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Save")
	bool IsIndexUnlocked(FName IndexName) const;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Save")
	bool IsIndexCompleted(FName IndexName) const;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Save")
	void SetProtocolValues(float NewMercyProtocol, float NewFear, float NewMercyResponse, float NewResistance, int32 NewDeathCount);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Save")
	void ResetSaveData();

private:
	int32 FindSavedIndex(FName IndexName) const;
	float ClampPercent(float Value) const;
};