#include "Index2HelpRoomController.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MercyDoorController.h"
#include "MercyHelpers.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AIndex2HelpRoomController::AIndex2HelpRoomController()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TrapTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TrapTrigger"));
	TrapTrigger->SetupAttachment(SceneRoot);
	TrapTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TrapTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	TrapTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	TrapTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AIndex2HelpRoomController::BeginPlay()
{
	Super::BeginPlay();

	if (TrapTrigger)
	{
		TrapTrigger->SetBoxExtent(TrapTriggerExtent);
		TrapTrigger->OnComponentBeginOverlap.AddDynamic(this, &AIndex2HelpRoomController::HandleTrapBeginOverlap);
	}

	UMercyHelpers::SetActorsHiddenByTag(this, SilentExitTag, true);

	DebugMessage(TEXT("Index2 Help Room Controller ready"), FColor::Cyan, 3.0f);
}

void AIndex2HelpRoomController::HandleTrapBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!UMercyHelpers::IsValidPlayerActor(OtherActor))
	{
		return;
	}

	if (bTriggerOnlyOnce && bHasTriggered)
	{
		return;
	}

	StartHelpTrap();
}

void AIndex2HelpRoomController::StartHelpTrap()
{
	bHasTriggered = true;

	CloseHelpRoomDoors();

	UMercyHelpers::SetLightIntensityByTag(this, MainLightTag, 0.0f);
	UMercyHelpers::SetLightIntensityByTag(this, RedLightTag, 700.0f);

	UMercyHelpers::PlaySound2DIfValid(this, TrapCloseSound);
	UMercyHelpers::PlaySound2DIfValid(this, HelpVoiceSound);

	GetWorldTimerManager().SetTimer(
		FirstMessageTimerHandle,
		this,
		&AIndex2HelpRoomController::ShowFirstMessage,
		FirstMessageDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		SecondMessageTimerHandle,
		this,
		&AIndex2HelpRoomController::ShowSecondMessage,
		SecondMessageDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		ExitRevealTimerHandle,
		this,
		&AIndex2HelpRoomController::RevealSilentExit,
		ExitRevealDelay,
		false
	);

	DebugMessage(TEXT("INDEX-2 HELP trap started"), FColor::Red, 5.0f);
}

void AIndex2HelpRoomController::ShowFirstMessage()
{
	ShowSystemText(TEXT("HELP RESPONSE DETECTED"));
}

void AIndex2HelpRoomController::ShowSecondMessage()
{
	ShowSystemText(TEXT("YOU CAME BECAUSE IT ASKED"));
}

void AIndex2HelpRoomController::RevealSilentExit()
{
	UMercyHelpers::SetActorsHiddenByTag(this, SilentExitTag, false);

	ShowSystemText(TEXT("SILENCE IS THE ONLY EXIT"), 6.0f);

	DebugMessage(TEXT("INDEX-2 silent exit revealed"), FColor::Green, 5.0f);
}

void AIndex2HelpRoomController::CloseHelpRoomDoors()
{
	TArray<AActor*> DoorActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), DoorControllerTag, DoorActors);

	for (AActor* Actor : DoorActors)
	{
		AMercyDoorController* DoorController = Cast<AMercyDoorController>(Actor);

		if (DoorController)
		{
			DoorController->CloseDoor();
			DoorController->LockDoor();
		}
	}
}

void AIndex2HelpRoomController::SetActorsHiddenByTag(FName Tag, bool bShouldHide)
{
	UMercyHelpers::SetActorsHiddenByTag(this, Tag, bShouldHide);
}

void AIndex2HelpRoomController::SetActorsCollisionByTag(FName Tag, bool bEnableCollision)
{
	if (Tag.IsNone())
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (Actor)
		{
			Actor->SetActorEnableCollision(bEnableCollision);
		}
	}
}

void AIndex2HelpRoomController::SetLightIntensityByTag(FName Tag, float NewIntensity)
{
	UMercyHelpers::SetLightIntensityByTag(this, Tag, NewIntensity);
}

void AIndex2HelpRoomController::ShowSystemText(const FString& Message, float AutoHideAfter)
{
	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, Message, TypewriterInterval, AutoHideAfter);
}

void AIndex2HelpRoomController::PlaySound2DIfValid(USoundBase* Sound)
{
	UMercyHelpers::PlaySound2DIfValid(this, Sound);
}

bool AIndex2HelpRoomController::IsValidPlayerActor(AActor* OtherActor) const
{
	return UMercyHelpers::IsValidPlayerActor(OtherActor);
}

void AIndex2HelpRoomController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UMercyHelpers::DebugMessage(bShowDebugMessages, Message, Color, Duration);
}
