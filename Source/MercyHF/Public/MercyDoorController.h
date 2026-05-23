#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercyDoorController.generated.h"

class UBoxComponent;
class USceneComponent;

UCLASS()
class MERCYHF_API AMercyDoorController : public AActor
{
	GENERATED_BODY()

public:
	AMercyDoorController();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* TriggerBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Door")
	AActor* TargetDoorActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Door")
	FName TargetDoorTag = TEXT("MERCY_DOOR");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Door")
	FString TargetDoorNameContains = TEXT("Door");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Door")
	bool bStartLocked = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Door")
	bool bStartOpen = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Door")
	bool bOpenOnPlayerOverlap = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Door")
	bool bCloseOnPlayerOverlap = false;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Door")
	bool bDisableCollisionWhenOpen = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Animation")
	float OpenYawOffset = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Animation")
	float DoorOpenSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	bool bDoorLocked = false;
	bool bDoorOpen = false;
	bool bAnimating = false;

	FRotator ClosedRotation;
	FRotator OpenRotation;

	UFUNCTION()
	void HandleTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Door")
	void OpenDoor();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Door")
	void CloseDoor();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Door")
	void ToggleDoor();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Door")
	void LockDoor();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Door")
	void UnlockDoor();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Door")
	bool IsDoorOpen() const;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Door")
	bool IsDoorLocked() const;

private:
	void CacheTargetDoor();
	void ApplyDoorCollision(bool bEnableCollision);
	bool IsValidPlayerActor(AActor* OtherActor) const;
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};