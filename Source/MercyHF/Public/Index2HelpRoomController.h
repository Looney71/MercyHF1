#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Index2HelpRoomController.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;

UCLASS()
class MERCYHF_API AIndex2HelpRoomController : public AActor
{
	GENERATED_BODY()

public:
	AIndex2HelpRoomController();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UBoxComponent* TrapTrigger = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	bool bTriggerOnlyOnce = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	FVector TrapTriggerExtent = FVector(350.0f, 350.0f, 220.0f);

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	FName SystemTextTag = TEXT("SYSTEM_TEXT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	FName DoorControllerTag = TEXT("HELP_ROOM_DOOR");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	FName MainLightTag = TEXT("HELP_ROOM_MAIN_LIGHT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	FName RedLightTag = TEXT("HELP_ROOM_RED_LIGHT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	FName SilentExitTag = TEXT("HELP_ROOM_SILENT_EXIT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	float FirstMessageDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	float SecondMessageDelay = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	float ExitRevealDelay = 12.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	float TypewriterInterval = 0.035f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	USoundBase* HelpVoiceSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Index 2")
	USoundBase* TrapCloseSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	bool bHasTriggered = false;

	FTimerHandle FirstMessageTimerHandle;
	FTimerHandle SecondMessageTimerHandle;
	FTimerHandle ExitRevealTimerHandle;

	UFUNCTION()
	void HandleTrapBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Index 2")
	void StartHelpTrap();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Index 2")
	void RevealSilentExit();

private:
	void ShowFirstMessage();
	void ShowSecondMessage();
	void CloseHelpRoomDoors();
	void SetActorsHiddenByTag(FName Tag, bool bShouldHide);
	void SetActorsCollisionByTag(FName Tag, bool bEnableCollision);
	void SetLightIntensityByTag(FName Tag, float NewIntensity);
	void ShowSystemText(const FString& Message, float AutoHideAfter = 0.0f);
	void PlaySound2DIfValid(USoundBase* Sound);
	bool IsValidPlayerActor(AActor* OtherActor) const;
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};