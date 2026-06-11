#include "MercyDoorController.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

AMercyDoorController::AMercyDoorController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneRoot);
	TriggerBox->SetBoxExtent(FVector(150.0f, 150.0f, 150.0f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMercyDoorController::BeginPlay()
{
	Super::BeginPlay();

	bDoorLocked = bStartLocked;

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMercyDoorController::HandleTriggerBeginOverlap);
	}

	CacheTargetDoor();

	if (!TargetDoorActor)
	{
		TargetDoorActor = this;
	}

	ClosedRotation = TargetDoorActor->GetActorRotation();
	OpenRotation = ClosedRotation + FRotator(0.0f, OpenYawOffset, 0.0f);

	if (bStartOpen)
	{
		TargetDoorActor->SetActorRotation(OpenRotation);
		bDoorOpen = true;
		ApplyDoorCollision(!bDisableCollisionWhenOpen);
	}
	else
	{
		TargetDoorActor->SetActorRotation(ClosedRotation);
		bDoorOpen = false;
		ApplyDoorCollision(true);
	}

	DebugMessage(TEXT("MercyDoorController ready"), FColor::Cyan, 3.0f);
}

void AMercyDoorController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bAnimating || !TargetDoorActor)
	{
		return;
	}

	const FRotator CurrentRotation = TargetDoorActor->GetActorRotation();
	const FRotator DesiredRotation = bDoorOpen ? OpenRotation : ClosedRotation;

	const FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		DesiredRotation,
		DeltaTime,
		DoorOpenSpeed
	);

	TargetDoorActor->SetActorRotation(NewRotation);

	if (NewRotation.Equals(DesiredRotation, 0.5f))
	{
		TargetDoorActor->SetActorRotation(DesiredRotation);
		bAnimating = false;

		if (bDoorOpen && bDisableCollisionWhenOpen)
		{
			ApplyDoorCollision(false);
		}

		if (!bDoorOpen)
		{
			ApplyDoorCollision(true);
		}
	}
}

void AMercyDoorController::HandleTriggerBeginOverlap(
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

	if (bOpenOnPlayerOverlap)
	{
		OpenDoor();
	}

	if (bCloseOnPlayerOverlap)
	{
		CloseDoor();
	}
}

void AMercyDoorController::OpenDoor()
{
	if (bDoorLocked)
	{
		DebugMessage(TEXT("Door is locked"), FColor::Red, 3.0f);
		return;
	}

	if (!TargetDoorActor)
	{
		DebugMessage(TEXT("Door target missing"), FColor::Red, 3.0f);
		return;
	}

	bDoorOpen = true;
	bAnimating = true;

	DebugMessage(TEXT("Door opening"), FColor::Green, 3.0f);
}

void AMercyDoorController::CloseDoor()
{
	if (!TargetDoorActor)
	{
		DebugMessage(TEXT("Door target missing"), FColor::Red, 3.0f);
		return;
	}

	ApplyDoorCollision(true);

	bDoorOpen = false;
	bAnimating = true;

	DebugMessage(TEXT("Door closing"), FColor::Yellow, 3.0f);
}

void AMercyDoorController::ToggleDoor()
{
	if (bDoorOpen)
	{
		CloseDoor();
	}
	else
	{
		OpenDoor();
	}
}

void AMercyDoorController::LockDoor()
{
	bDoorLocked = true;
	DebugMessage(TEXT("Door locked"), FColor::Red, 3.0f);
}

void AMercyDoorController::UnlockDoor()
{
	bDoorLocked = false;
	DebugMessage(TEXT("Door unlocked"), FColor::Green, 3.0f);
}

bool AMercyDoorController::IsDoorOpen() const
{
	return bDoorOpen;
}

bool AMercyDoorController::IsDoorLocked() const
{
	return bDoorLocked;
}

void AMercyDoorController::CacheTargetDoor()
{
	if (TargetDoorActor)
	{
		return;
	}

	// PERFORMANCE OPTIMIZATION:
	// Avoid unnecessary TArray heap allocations by using TActorRange instead of GetAllActorsOfClass
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		if (!Actor || Actor == this)
		{
			continue;
		}

		if (!TargetDoorTag.IsNone() && Actor->ActorHasTag(TargetDoorTag))
		{
			TargetDoorActor = Actor;
			return;
		}

		const FString ActorName = Actor->GetName();

		if (!TargetDoorNameContains.IsEmpty() && ActorName.Contains(TargetDoorNameContains))
		{
			TargetDoorActor = Actor;
			return;
		}

#if WITH_EDITOR
		const FString ActorEditorLabel = Actor->GetActorLabel();

		if (!TargetDoorNameContains.IsEmpty() && ActorEditorLabel.Contains(TargetDoorNameContains))
		{
			TargetDoorActor = Actor;
			return;
		}
#endif
	}
}

void AMercyDoorController::ApplyDoorCollision(bool bEnableCollision)
{
	if (!TargetDoorActor)
	{
		return;
	}

	TargetDoorActor->SetActorEnableCollision(bEnableCollision);
}

bool AMercyDoorController::IsValidPlayerActor(AActor* OtherActor) const
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

void AMercyDoorController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}