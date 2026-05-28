#include "Index4FirstDeathController.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MercyHelpers.h"
#include "MercyProtocolComponent.h"
#include "MercyRespawnComponent.h"
#include "MercySystemTextActor.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AIndex4FirstDeathController::AIndex4FirstDeathController()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DeathTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("DeathTrigger"));
	DeathTrigger->SetupAttachment(SceneRoot);
	DeathTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DeathTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	DeathTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	DeathTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AIndex4FirstDeathController::BeginPlay()
{
	Super::BeginPlay();

	if (DeathTrigger)
	{
		DeathTrigger->SetBoxExtent(DeathTriggerExtent);
		DeathTrigger->OnComponentBeginOverlap.AddDynamic(this, &AIndex4FirstDeathController::HandleDeathTriggerOverlap);
	}

	UMercyHelpers::SetActorsHiddenByTag(this, MonsterRevealTag, true);

	DebugMessage(TEXT("Index4 First Death Controller ready"), FColor::Cyan, 3.0f);
}

void AIndex4FirstDeathController::HandleDeathTriggerOverlap(
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

	StartFirstDeath(OtherActor);
}

void AIndex4FirstDeathController::StartFirstDeath(AActor* VictimActor)
{
	if (!VictimActor)
	{
		return;
	}

	bHasTriggered = true;
	PendingVictim = VictimActor;

	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, TEXT("ESCAPE ATTEMPT DETECTED"));
	UMercyHelpers::SetActorsHiddenByTag(this, FakeExitTag, false);

	GetWorldTimerManager().SetTimer(
		ChaseTimerHandle,
		this,
		&AIndex4FirstDeathController::StartChase,
		ChaseStartDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		TerminationTimerHandle,
		this,
		&AIndex4FirstDeathController::TerminateSubject,
		TerminationDelay,
		false
	);

	DebugMessage(TEXT("INDEX-4 first death sequence started"), FColor::Red, 5.0f);
}

void AIndex4FirstDeathController::StartChase()
{
	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, TEXT("REGISTRATION INCOMPLETE"));
	UMercyHelpers::SetActorsHiddenByTag(this, MonsterRevealTag, false);
	UMercyHelpers::PlaySound2DIfValid(this, ChaseSound);

	DebugMessage(TEXT("INDEX-4 chase started"), FColor::Red, 4.0f);
}

void AIndex4FirstDeathController::TerminateSubject()
{
	AActor* VictimActor = PendingVictim.Get();

	if (!VictimActor)
	{
		return;
	}

	UMercyProtocolComponent* ProtocolComponent = VictimActor->FindComponentByClass<UMercyProtocolComponent>();

	if (ProtocolComponent)
	{
		ProtocolComponent->AddDeath();
		ProtocolComponent->AddMercyProtocolPercent(5.0f);
		ProtocolComponent->AddFearPercent(15.0f);
	}

	UMercyHelpers::PlaySound2DIfValid(this, DeathSound);

	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, TEXT("SUBJECT TERMINATED"));
	UMercyHelpers::SetActorsHiddenByTag(this, MonsterRevealTag, true);

	GetWorldTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&AIndex4FirstDeathController::RestoreSubject,
		RespawnDelay,
		false
	);

	DebugMessage(TEXT("INDEX-4 subject terminated"), FColor::Red, 5.0f);
}

void AIndex4FirstDeathController::RestoreSubject()
{
	AActor* VictimActor = PendingVictim.Get();

	if (!VictimActor)
	{
		return;
	}

	UMercyRespawnComponent* RespawnComponent = VictimActor->FindComponentByClass<UMercyRespawnComponent>();

	if (RespawnComponent)
	{
		RespawnComponent->RespawnOwnerAtCheckpoint();
	}

	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, TEXT("DEATH IS NOT AN EXIT"), 0.035f, 6.0f);

	DebugMessage(TEXT("INDEX-4 subject restored"), FColor::Green, 5.0f);
}

void AIndex4FirstDeathController::ShowSystemText(const FString& Message, float AutoHideAfter)
{
	UMercyHelpers::ShowSystemTextByTag(this, SystemTextTag, Message, 0.035f, AutoHideAfter);
}

void AIndex4FirstDeathController::SetActorsHiddenByTag(FName Tag, bool bShouldHide)
{
	UMercyHelpers::SetActorsHiddenByTag(this, Tag, bShouldHide);
}

void AIndex4FirstDeathController::PlaySound2DIfValid(USoundBase* Sound)
{
	UMercyHelpers::PlaySound2DIfValid(this, Sound);
}

bool AIndex4FirstDeathController::IsValidPlayerActor(AActor* OtherActor) const
{
	return UMercyHelpers::IsValidPlayerActor(OtherActor);
}

void AIndex4FirstDeathController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UMercyHelpers::DebugMessage(bShowDebugMessages, Message, Color, Duration);
}