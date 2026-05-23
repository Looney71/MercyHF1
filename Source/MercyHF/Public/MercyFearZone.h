#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyFearZone.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyFearZoneTriggered, AActor*, TriggeringActor);

UCLASS()
class MERCYHF_API AMercyFearZone : public AActor
{
	GENERATED_BODY()

public:
	AMercyFearZone();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* FearZoneBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Fear")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Fear")
	bool bTriggerOnlyOnce = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Fear")
	FVector FearZoneExtent = FVector(300.0f, 300.0f, 200.0f);

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Fear")
	float FearDelta = 8.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Fear")
	float MercyProtocolDelta = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Fear")
	float ResistanceDelta = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* FearSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	bool bPlaySound2D = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	bool bHasTriggered = false;

	UFUNCTION()
	void HandleZoneBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Fear")
	FMercyFearZoneTriggered OnFearZoneTriggered;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Fear")
	void EnableFearZone();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Fear")
	void DisableFearZone();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Fear")
	void ResetFearZone();

private:
	void ApplyFearToActor(AActor* Actor);
	bool IsValidPlayerActor(AActor* OtherActor) const;

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};