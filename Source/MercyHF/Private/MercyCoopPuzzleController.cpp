#include "MercyCoopPuzzleController.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AMercyCoopPuzzleController::AMercyCoopPuzzleController()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PuzzleZone = CreateDefaultSubobject<UBoxComponent>(TEXT("PuzzleZone"));
	PuzzleZone->SetupAttachment(SceneRoot);
	PuzzleZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PuzzleZone->SetCollisionObjectType(ECC_WorldDynamic);
	PuzzleZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	PuzzleZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMercyCoopPuzzleController::BeginPlay()
{
	Super::BeginPlay();

	if (PuzzleZone)
	{
		PuzzleZone->SetBoxExtent(PuzzleZoneExtent);
		PuzzleZone->OnComponentBeginOverlap.AddDynamic(this, &AMercyCoopPuzzleController::HandleZoneBeginOverlap);
		PuzzleZone->OnComponentEndOverlap.AddDynamic(this, &AMercyCoopPuzzleController::HandleZoneEndOverlap);
	}

	DebugMessage(TEXT("MercyCoopPuzzleController ready"), FColor::Cyan, 3.0f);
}

void AMercyCoopPuzzleController::HandleZoneBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IsValidPlayerActor(OtherActor))
	{
		return;
	}

	PlayersInside.AddUnique(OtherActor);
	EvaluatePuzzleState();
}

void AMercyCoopPuzzleController::HandleZoneEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex
)
{
	if (!IsValidPlayerActor(OtherActor))
	{
		return;
	}

	PlayersInside.Remove(OtherActor);
}

void AMercyCoopPuzzleController::EvaluatePuzzleState()
{
	if (bSolved && bSolveOnlyOnce)
	{
		return;
	}

	DebugMessage(
		FString::Printf(TEXT("Coop puzzle players: %d / %d"), PlayersInside.Num(), RequiredPlayers),
		FColor::Yellow,
		2.0f
	);

	if (PlayersInside.Num() >= RequiredPlayers)
	{
		if (SolveDelay > 0.0f)
		{
			GetWorldTimerManager().SetTimer(
				SolveTimerHandle,
				this,
				&AMercyCoopPuzzleController::SolvePuzzle,
				SolveDelay,
				false
			);
		}
		else
		{
			SolvePuzzle();
		}
	}
}

void AMercyCoopPuzzleController::SolvePuzzle()
{
	if (bSolved && bSolveOnlyOnce)
	{
		return;
	}

	if (PlayersInside.Num() < RequiredPlayers)
	{
		DebugMessage(TEXT("Coop puzzle solve cancelled: not enough players"), FColor::Orange, 3.0f);
		return;
	}

	bSolved = true;

	ApplySolvedActions();

	if (SolvedSound)
	{
		UGameplayStatics::PlaySound2D(this, SolvedSound);
	}

	OnPuzzleSolved.Broadcast();

	DebugMessage(TEXT("Coop puzzle solved"), FColor::Green, 5.0f);
}

void AMercyCoopPuzzleController::ResetPuzzle()
{
	bSolved = false;
	PlayersInside.Empty();
	GetWorldTimerManager().ClearTimer(SolveTimerHandle);

	DebugMessage(TEXT("Coop puzzle reset"), FColor::Cyan, 3.0f);
}

void AMercyCoopPuzzleController::ForceSolvePuzzle()
{
	bSolved = false;
	SolvePuzzle();
}

bool AMercyCoopPuzzleController::IsPuzzleSolved() const
{
	return bSolved;
}

int32 AMercyCoopPuzzleController::GetPlayersInsideCount() const
{
	return PlayersInside.Num();
}

void AMercyCoopPuzzleController::ApplySolvedActions()
{
	SetActorsHidden(ActorsToShowOnSolved, false);
	SetActorsHidden(ActorsToHideOnSolved, true);

	SetActorsHiddenByTag(ActorsToShowOnSolvedTag, false);
	SetActorsHiddenByTag(ActorsToHideOnSolvedTag, true);
}

void AMercyCoopPuzzleController::SetActorsHidden(const TArray<AActor*>& Actors, bool bShouldHide)
{
	for (AActor* Actor : Actors)
	{
		if (Actor)
		{
			Actor->SetActorHiddenInGame(bShouldHide);
			Actor->SetActorEnableCollision(!bShouldHide);
		}
	}
}

void AMercyCoopPuzzleController::SetActorsHiddenByTag(FName Tag, bool bShouldHide)
{
	if (Tag.IsNone())
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	SetActorsHidden(FoundActors, bShouldHide);
}

bool AMercyCoopPuzzleController::IsValidPlayerActor(AActor* OtherActor) const
{
	if (!OtherActor)
	{
		return false;
	}

	if (OtherActor->IsA<APawn>())
	{
		return true;
	}

	return OtherActor->ActorHasTag(TEXT("Player"));
}

void AMercyCoopPuzzleController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}