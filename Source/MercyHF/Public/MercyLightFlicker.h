#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyLightFlicker.generated.h"

class ULightComponent;

UCLASS()
class MERCYHF_API AMercyLightFlicker : public AActor
{
	GENERATED_BODY()

public:
	AMercyLightFlicker();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker")
	bool bStartFlickeringOnBeginPlay = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker")
	bool bCacheLightsOnBeginPlay = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker")
	FName TargetLightTag = TEXT("MERCY_FLICKER_LIGHT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker")
	FString TargetLightNameContains = TEXT("Light");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker")
	TArray<AActor*> TargetLightActors;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker", meta = (ClampMin = "0.0"))
	float MinIntensity = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker", meta = (ClampMin = "0.0"))
	float MaxIntensity = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker", meta = (ClampMin = "0.01"))
	float MinFlickerInterval = 0.04f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker", meta = (ClampMin = "0.01"))
	float MaxFlickerInterval = 0.18f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker")
	bool bSometimesTurnOffCompletely = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FullBlackoutChance = 0.18f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker")
	bool bRestoreIntensityWhenStopped = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Light Flicker", meta = (ClampMin = "0.0"))
	float RestoredIntensity = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	bool bIsFlickering = false;

	FTimerHandle FlickerTimerHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Light Flicker")
	void StartFlicker();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Light Flicker")
	void StopFlicker();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Light Flicker")
	void PulseOnce(float PulseIntensity = 1000.0f, float ReturnDelay = 0.15f);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Light Flicker")
	void SetAllLightIntensity(float NewIntensity);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Light Flicker")
	void CacheTargetLights();

private:
	void FlickerTick();
	void RestoreAfterPulse();

	void ApplyIntensityToActor(AActor* Actor, float NewIntensity);
	bool ActorMatchesTarget(AActor* Actor) const;

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};