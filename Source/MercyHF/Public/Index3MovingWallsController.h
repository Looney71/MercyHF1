#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Index3MovingWallsController.generated.h"

class USoundBase;

UENUM(BlueprintType)
enum class EMercyWallMovementPattern : uint8
{
	Sequential		UMETA(DisplayName = "Sequential"),
	Random			UMETA(DisplayName = "Random"),
	Synchronized	UMETA(DisplayName = "Synchronized"),
	PlayerTriggered	UMETA(DisplayName = "Player Triggered")
};

UENUM(BlueprintType)
enum class EMercyWallMovementType : uint8
{
	Slide			UMETA(DisplayName = "Slide"),
	Rotate			UMETA(DisplayName = "Rotate"),
	Scale			UMETA(DisplayName = "Scale"),
	Disappear		UMETA(DisplayName = "Disappear")
};

/**
 * Controller for INDEX-3 Moving Walls level.
 * Handles spatial horror mechanics with moving walls, corridor changes,
 * and psychological disorientation effects.
 */
UCLASS()
class MERCYHF_API AIndex3MovingWallsController : public AActor
{
	GENERATED_BODY()

public:
	AIndex3MovingWallsController();

protected:
	virtual void BeginPlay() override;

private:
	// Timing settings
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float InitialDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float FirstWallMoveDelay = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float CorridorChangeDelay = 8.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float Disorientation1Delay = 12.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float Disorientation2Delay = 18.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Timing")
	float FinalRevealDelay = 25.0f;

	// Wall movement settings
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Walls")
	EMercyWallMovementPattern MovementPattern = EMercyWallMovementPattern::Sequential;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Walls")
	EMercyWallMovementType MovementType = EMercyWallMovementType::Slide;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Walls")
	float WallMoveSpeed = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Walls")
	float WallMoveDistance = 400.0f;

	// Actor tags for different wall groups
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName LeftWallsTag = TEXT("INDEX3_LEFT_WALLS");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName RightWallsTag = TEXT("INDEX3_RIGHT_WALLS");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName CorridorWallsTag = TEXT("INDEX3_CORRIDOR_WALLS");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName FakeWallsTag = TEXT("INDEX3_FAKE_WALLS");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName SystemTextTag = TEXT("SYSTEM_TEXT");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Tags")
	FName PathLightsTag = TEXT("INDEX3_PATH_LIGHTS");

	// Audio settings
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* WallMoveSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* CorridorChangeSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	USoundBase* DisorientationSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Audio")
	FString WallMoveSoundPath = TEXT("/Game/MercyMaze/Audio/SFX_WallMove.SFX_WallMove");

	// System messages
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Messages")
	FString InitialMessage = TEXT("SPATIAL RECONFIGURATION INITIATED");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Messages")
	FString CorridorChangeMessage = TEXT("CORRIDOR LAYOUT MODIFIED");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Messages")
	FString DisorientationMessage = TEXT("PERCEPTION ADJUSTMENT IN PROGRESS");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Messages")
	FString FinalMessage = TEXT("SPATIAL MAZE PROTOCOL COMPLETE");

	// Debug settings
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	// Internal state
	UPROPERTY()
	TArray<AActor*> LeftWallActors;

	UPROPERTY()
	TArray<AActor*> RightWallActors;

	UPROPERTY()
	TArray<AActor*> CorridorWallActors;

	UPROPERTY()
	TArray<AActor*> FakeWallActors;

	UPROPERTY()
	TArray<AActor*> PathLightActors;

	// Timer handles
	FTimerHandle InitialTimerHandle;
	FTimerHandle FirstWallMoveTimerHandle;
	FTimerHandle CorridorChangeTimerHandle;
	FTimerHandle Disorientation1TimerHandle;
	FTimerHandle Disorientation2TimerHandle;
	FTimerHandle FinalRevealTimerHandle;

	// Sequence functions
	void CacheWallActors();
	void StartSpatialSequence();
	void ExecuteFirstWallMove();
	void ExecuteCorridorChange();
	void ExecuteDisorientation1();
	void ExecuteDisorientation2();
	void ExecuteFinalReveal();

	// Wall manipulation functions
	void MoveWallsInDirection(const TArray<AActor*>& Walls, const FVector& Direction, float Distance);
	void RotateWalls(const TArray<AActor*>& Walls, const FRotator& Rotation);
	void ScaleWalls(const TArray<AActor*>& Walls, const FVector& Scale);
	void SetWallsVisibility(const TArray<AActor*>& Walls, bool bVisible);

	// Utility functions
	bool ActorMatches(AActor* Actor, FName RequiredTag, const FString& NameContains) const;
	void ShowSystemMessage(const FString& Message);
	void PlaySpatialSound(USoundBase* Sound, const FString& SoundPath);
	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};