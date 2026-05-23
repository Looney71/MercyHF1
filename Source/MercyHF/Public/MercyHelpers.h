#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MercyHelpers.generated.h"

class USoundBase;

UCLASS()
class MERCYHF_API UMercyHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Helpers", meta = (WorldContext = "WorldContextObject"))
	static void SetActorsHiddenByTag(const UObject* WorldContextObject, FName Tag, bool bHidden, bool bUpdateCollision = true);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Helpers", meta = (WorldContext = "WorldContextObject"))
	static void SetLightIntensityByTag(const UObject* WorldContextObject, FName Tag, float NewIntensity);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Helpers", meta = (WorldContext = "WorldContextObject"))
	static void ShowSystemTextByTag(
		const UObject* WorldContextObject,
		FName Tag,
		const FString& Message,
		float CharacterIntervalSeconds = 0.035f,
		float AutoHideAfterSeconds = 0.0f
	);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Helpers", meta = (WorldContext = "WorldContextObject"))
	static void PlaySound2DIfValid(const UObject* WorldContextObject, USoundBase* Sound);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Helpers")
	static bool IsValidPlayerActor(AActor* OtherActor);

	static void DebugMessage(bool bShowOnScreen, const FString& Message, const FColor& Color, float Duration);
};
