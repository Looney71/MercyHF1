#include "MercySaveGame.h"

UMercySaveGame::UMercySaveGame()
{
	LastSaveUtcTime = FDateTime::UtcNow().ToString();

	UnlockedIndexes.AddUnique(TEXT("INDEX_0_ENTRY"));
}

void UMercySaveGame::MarkIndexUnlocked(FName IndexName)
{
	if (IndexName.IsNone())
	{
		return;
	}

	UnlockedIndexes.AddUnique(IndexName);
	LastUnlockedIndex = IndexName;
	LastSaveUtcTime = FDateTime::UtcNow().ToString();
}

void UMercySaveGame::MarkIndexCompleted(FName IndexName, float CompletionTimeSeconds, int32 DeathsInIndex)
{
	if (IndexName.IsNone())
	{
		return;
	}

	CompletedIndexes.AddUnique(IndexName);
	UnlockedIndexes.AddUnique(IndexName);

	const int32 ExistingIndex = FindSavedIndex(IndexName);

	if (ExistingIndex == INDEX_NONE)
	{
		FMercySavedIndexData NewData;
		NewData.IndexName = IndexName;
		NewData.bCompleted = true;
		NewData.BestCompletionTimeSeconds = FMath::Max(0.0f, CompletionTimeSeconds);
		NewData.DeathsInIndex = FMath::Max(0, DeathsInIndex);

		SavedIndexes.Add(NewData);
	}
	else
	{
		FMercySavedIndexData& ExistingData = SavedIndexes[ExistingIndex];

		ExistingData.bCompleted = true;
		ExistingData.DeathsInIndex = FMath::Max(0, DeathsInIndex);

		if (ExistingData.BestCompletionTimeSeconds <= 0.0f)
		{
			ExistingData.BestCompletionTimeSeconds = FMath::Max(0.0f, CompletionTimeSeconds);
		}
		else
		{
			ExistingData.BestCompletionTimeSeconds = FMath::Min(
				ExistingData.BestCompletionTimeSeconds,
				FMath::Max(0.0f, CompletionTimeSeconds)
			);
		}
	}

	LastPlayedIndex = IndexName;
	LastSaveUtcTime = FDateTime::UtcNow().ToString();
}

bool UMercySaveGame::IsIndexUnlocked(FName IndexName) const
{
	return UnlockedIndexes.Contains(IndexName);
}

bool UMercySaveGame::IsIndexCompleted(FName IndexName) const
{
	return CompletedIndexes.Contains(IndexName);
}

void UMercySaveGame::SetProtocolValues(float NewMercyProtocol, float NewFear, float NewMercyResponse, float NewResistance, int32 NewDeathCount)
{
	MercyProtocolPercent = ClampPercent(NewMercyProtocol);
	FearPercent = ClampPercent(NewFear);
	MercyResponsePercent = ClampPercent(NewMercyResponse);
	ResistancePercent = ClampPercent(NewResistance);
	TotalDeathCount = FMath::Max(0, NewDeathCount);

	LastSaveUtcTime = FDateTime::UtcNow().ToString();
}

void UMercySaveGame::ResetSaveData()
{
	LastUnlockedIndex = TEXT("INDEX_0_ENTRY");
	LastPlayedIndex = TEXT("INDEX_0_ENTRY");

	MercyProtocolPercent = 0.0f;
	FearPercent = 0.0f;
	MercyResponsePercent = 0.0f;
	ResistancePercent = 0.0f;
	TotalDeathCount = 0;

	SavedIndexes.Empty();
	UnlockedIndexes.Empty();
	CompletedIndexes.Empty();

	UnlockedIndexes.AddUnique(TEXT("INDEX_0_ENTRY"));

	LastSaveUtcTime = FDateTime::UtcNow().ToString();
}

int32 UMercySaveGame::FindSavedIndex(FName IndexName) const
{
	for (int32 Index = 0; Index < SavedIndexes.Num(); ++Index)
	{
		if (SavedIndexes[Index].IndexName == IndexName)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

float UMercySaveGame::ClampPercent(float Value) const
{
	return FMath::Clamp(Value, 0.0f, 100.0f);
}