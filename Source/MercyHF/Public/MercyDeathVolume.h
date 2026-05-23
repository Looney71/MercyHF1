#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyDeathVolume.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyDeathVolumeTriggered, AActor*, VictimActor);

UCLASS()
class MERCYHF_API AMercyDeathVolume : public AActor
{
	GENERATED_BODY()

public:
	AMercyDeathVolume();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* DeathBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Death")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Death")
	float RespawnDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Death")
	FVector DeathBoxExtent = FVector(300.0f, 300.0f, 200.0f);

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Death")
	AActor* FallbackRespawnTarget = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Death")
	FName SystemTextTag = TEXT("SYSTEM_TEXT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Death")
	FString DeathMessage = TEXT("DEATH IS NOT AN EXIT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* DeathSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	UFUNCTION()
	void HandleDeathBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Death")
	FMercyDeathVolumeTriggered OnDeathVolumeTriggered;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Death")
	void EnableDeathVolume();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Death")
	void DisableDeathVolume();

private:
	void ProcessDeath(AActor* VictimActor);
	void RespawnActor(AActor* VictimActor);
	void ShowDeathText();
	bool IsValidPlayerActor(AActor* OtherActor) const;

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};