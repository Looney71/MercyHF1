#include "Index3MovingWallsController.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "MercyHelpers.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "EngineUtils.h"

AIndex3MovingWallsController::AIndex3MovingWallsController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AIndex3MovingWallsController::BeginPlay()
{
	Super::BeginPlay();

	DebugMessage(TEXT("=== INDEX-3 MOVING WALLS CONTROLLER STARTING ==="), FColor::Green, 6.0f);
	DebugMessage(FString::Printf(TEXT("Controller placed at: %s"), *GetActorLocation().ToString()), FColor::Green, 4.0f);

	CacheWallActors();

	// Set up the spatial sequence with initial delay
	GetWorldTimerManager().SetTimer(
		InitialTimerHandle,
		this,
		&AIndex3MovingWallsController::StartSpatialSequence,
		InitialDelay,
		false
	);

	DebugMessage(FString::Printf(TEXT("Spatial sequence will begin in %.1f seconds"), InitialDelay), FColor::Blue, 5.0f);
	DebugMessage(TEXT("=== INDEX-3 MOVING WALLS CONTROLLER READY ==="), FColor::Green, 6.0f);
}

void AIndex3MovingWallsController::CacheWallActors()
{
	LeftWallActors.Empty();
	RightWallActors.Empty();
	CorridorWallActors.Empty();
	FakeWallActors.Empty();
	PathLightActors.Empty();

	DebugMessage(TEXT("=== INDEX-3 WALL CACHING START ==="), FColor::White, 8.0f);

	// ⚡ Bolt Performance Optimization:
	// Replacing UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors)
	// with TActorRange to avoid large TArray heap allocations when iterating over all actors.
	int32 TotalActorsCount = 0;

	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!Actor)
		{
			continue;
		}

		// Cache left walls
		if (ActorMatches(Actor, LeftWallsTag, TEXT("LeftWall")))
		{
			LeftWallActors.Add(Actor);
			DebugMessage(FString::Printf(TEXT("Left wall found: %s"), *Actor->GetName()), FColor::Green, 6.0f);
		}

		// Cache right walls
		if (ActorMatches(Actor, RightWallsTag, TEXT("RightWall")))
		{
			RightWallActors.Add(Actor);
			DebugMessage(FString::Printf(TEXT("Right wall found: %s"), *Actor->GetName()), FColor::Green, 6.0f);
		}

		// Cache corridor walls
		if (ActorMatches(Actor, CorridorWallsTag, TEXT("CorridorWall")))
		{
			CorridorWallActors.Add(Actor);
			DebugMessage(FString::Printf(TEXT("Corridor wall found: %s"), *Actor->GetName()), FColor::Green, 6.0f);
		}

		// Cache fake walls
		if (ActorMatches(Actor, FakeWallsTag, TEXT("FakeWall")))
		{
			FakeWallActors.Add(Actor);
			DebugMessage(FString::Printf(TEXT("Fake wall found: %s"), *Actor->GetName()), FColor::Green, 6.0f);
		}

		// Cache path lights
		if (ActorMatches(Actor, PathLightsTag, TEXT("PathLight")))
		{
			PathLightActors.Add(Actor);
			DebugMessage(FString::Printf(TEXT("Path light found: %s"), *Actor->GetName()), FColor::Green, 6.0f);
		}

		TotalActorsCount++;
	}

	DebugMessage(FString::Printf(TEXT("Total actors in level: %d"), TotalActorsCount), FColor::White, 6.0f);

	// Summary report
	DebugMessage(FString::Printf(TEXT("LEFT WALLS: %d found"), LeftWallActors.Num()), FColor::Cyan, 8.0f);
	DebugMessage(FString::Printf(TEXT("RIGHT WALLS: %d found"), RightWallActors.Num()), FColor::Cyan, 8.0f);
	DebugMessage(FString::Printf(TEXT("CORRIDOR WALLS: %d found"), CorridorWallActors.Num()), FColor::Cyan, 8.0f);
	DebugMessage(FString::Printf(TEXT("FAKE WALLS: %d found"), FakeWallActors.Num()), FColor::Cyan, 8.0f);
	DebugMessage(FString::Printf(TEXT("PATH LIGHTS: %d found"), PathLightActors.Num()), FColor::Cyan, 8.0f);

	DebugMessage(TEXT("=== INDEX-3 WALL CACHING COMPLETE ==="), FColor::White, 8.0f);
}

void AIndex3MovingWallsController::StartSpatialSequence()
{
	DebugMessage(TEXT("=== SPATIAL RECONFIGURATION SEQUENCE STARTING ==="), FColor::Red, 6.0f);

	ShowSystemMessage(InitialMessage);

	// Set up the sequence timers
	GetWorldTimerManager().SetTimer(
		FirstWallMoveTimerHandle,
		this,
		&AIndex3MovingWallsController::ExecuteFirstWallMove,
		FirstWallMoveDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		CorridorChangeTimerHandle,
		this,
		&AIndex3MovingWallsController::ExecuteCorridorChange,
		CorridorChangeDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		Disorientation1TimerHandle,
		this,
		&AIndex3MovingWallsController::ExecuteDisorientation1,
		Disorientation1Delay,
		false
	);

	GetWorldTimerManager().SetTimer(
		Disorientation2TimerHandle,
		this,
		&AIndex3MovingWallsController::ExecuteDisorientation2,
		Disorientation2Delay,
		false
	);

	GetWorldTimerManager().SetTimer(
		FinalRevealTimerHandle,
		this,
		&AIndex3MovingWallsController::ExecuteFinalReveal,
		FinalRevealDelay,
		false
	);

	DebugMessage(TEXT("All spatial sequence timers set"), FColor::Blue, 5.0f);
}

void AIndex3MovingWallsController::ExecuteFirstWallMove()
{
	DebugMessage(TEXT("=== FIRST WALL MOVEMENT ==="), FColor::Orange, 5.0f);

	PlaySpatialSound(WallMoveSound, WallMoveSoundPath);

	// Move left walls inward to create claustrophobia
	MoveWallsInDirection(LeftWallActors, FVector(0, WallMoveDistance, 0), WallMoveDistance);

	DebugMessage(FString::Printf(TEXT("Moved %d left walls inward"), LeftWallActors.Num()), FColor::Orange, 4.0f);
}

void AIndex3MovingWallsController::ExecuteCorridorChange()
{
	DebugMessage(TEXT("=== CORRIDOR RECONFIGURATION ==="), FColor::Purple, 5.0f);

	ShowSystemMessage(CorridorChangeMessage);
	PlaySpatialSound(CorridorChangeSound, TEXT(""));

	// Rotate corridor walls to change the path
	RotateWalls(CorridorWallActors, FRotator(0.0f, 90.0f, 0.0f));

	// Move right walls to create new passages
	MoveWallsInDirection(RightWallActors, FVector(0, -WallMoveDistance, 0), WallMoveDistance);

	DebugMessage(FString::Printf(TEXT("Reconfigured %d corridor walls and %d right walls"),
		CorridorWallActors.Num(), RightWallActors.Num()), FColor::Purple, 4.0f);
}

void AIndex3MovingWallsController::ExecuteDisorientation1()
{
	DebugMessage(TEXT("=== DISORIENTATION PHASE 1 ==="), FColor::Magenta, 5.0f);

	ShowSystemMessage(DisorientationMessage);
	PlaySpatialSound(DisorientationSound, TEXT(""));

	// Make fake walls appear to confuse the player
	SetWallsVisibility(FakeWallActors, true);

	// Dim path lights to increase confusion
	UMercyHelpers::SetLightIntensityByTag(this, PathLightsTag, 50.0f);

	DebugMessage(FString::Printf(TEXT("Revealed %d fake walls, dimmed path lights"), FakeWallActors.Num()), FColor::Magenta, 4.0f);
}

void AIndex3MovingWallsController::ExecuteDisorientation2()
{
	DebugMessage(TEXT("=== DISORIENTATION PHASE 2 ==="), FColor::Red, 5.0f);

	PlaySpatialSound(WallMoveSound, WallMoveSoundPath);

	// Scale walls to create size distortion
	ScaleWalls(LeftWallActors, FVector(1.0f, 1.0f, 1.5f));
	ScaleWalls(RightWallActors, FVector(1.0f, 1.0f, 0.7f));

	// Hide some fake walls to reveal the true path
	SetWallsVisibility(FakeWallActors, false);

	DebugMessage(TEXT("Applied scale distortion and revealed true path"), FColor::Red, 4.0f);
}

void AIndex3MovingWallsController::ExecuteFinalReveal()
{
	DebugMessage(TEXT("=== FINAL SPATIAL REVEAL ==="), FColor::Green, 5.0f);

	ShowSystemMessage(FinalMessage);

	// Restore normal lighting
	UMercyHelpers::SetLightIntensityByTag(this, PathLightsTag, 200.0f);

	// Reset wall scales
	ScaleWalls(LeftWallActors, FVector(1.0f, 1.0f, 1.0f));
	ScaleWalls(RightWallActors, FVector(1.0f, 1.0f, 1.0f));

	DebugMessage(TEXT("Spatial maze protocol complete - path revealed"), FColor::Green, 6.0f);
}

void AIndex3MovingWallsController::MoveWallsInDirection(const TArray<AActor*>& Walls, const FVector& Direction, float Distance)
{
	for (AActor* Wall : Walls)
	{
		if (Wall)
		{
			const FVector CurrentLocation = Wall->GetActorLocation();
			const FVector NewLocation = CurrentLocation + (Direction.GetSafeNormal() * Distance);
			Wall->SetActorLocation(NewLocation);
		}
	}
}

void AIndex3MovingWallsController::RotateWalls(const TArray<AActor*>& Walls, const FRotator& Rotation)
{
	for (AActor* Wall : Walls)
	{
		if (Wall)
		{
			const FRotator CurrentRotation = Wall->GetActorRotation();
			const FRotator NewRotation = CurrentRotation + Rotation;
			Wall->SetActorRotation(NewRotation);
		}
	}
}

void AIndex3MovingWallsController::ScaleWalls(const TArray<AActor*>& Walls, const FVector& Scale)
{
	for (AActor* Wall : Walls)
	{
		if (Wall)
		{
			Wall->SetActorScale3D(Scale);
		}
	}
}

void AIndex3MovingWallsController::SetWallsVisibility(const TArray<AActor*>& Walls, bool bVisible)
{
	for (AActor* Wall : Walls)
	{
		if (Wall)
		{
			Wall->SetActorHiddenInGame(!bVisible);
			Wall->SetActorEnableCollision(bVisible);
		}
	}
}

bool AIndex3MovingWallsController::ActorMatches(AActor* Actor, FName RequiredTag, const FString& NameContains) const
{
	if (!Actor)
	{
		return false;
	}

	if (!RequiredTag.IsNone() && Actor->ActorHasTag(RequiredTag))
	{
		return true;
	}

	const FString ActorName = Actor->GetName();
	if (ActorName.Contains(NameContains))
	{
		return true;
	}

#if WITH_EDITOR
	const FString ActorEditorLabel = Actor->GetActorLabel();
	if (ActorEditorLabel.Contains(NameContains))
	{
		return true;
	}
#endif

	return false;
}

void AIndex3MovingWallsController::ShowSystemMessage(const FString& Message)
{
	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, Message, 0.035f, 4.0f);
}

void AIndex3MovingWallsController::PlaySpatialSound(USoundBase* Sound, const FString& SoundPath)
{
	USoundBase* SoundToPlay = Sound;

	if (!SoundToPlay && !SoundPath.IsEmpty())
	{
		SoundToPlay = Cast<USoundBase>(
			StaticLoadObject(USoundBase::StaticClass(), nullptr, *SoundPath)
		);
	}

	if (SoundToPlay)
	{
		UGameplayStatics::PlaySound2D(this, SoundToPlay);
		DebugMessage(TEXT("Spatial sound played"), FColor::Purple, 3.0f);
	}
}

void AIndex3MovingWallsController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}