#include "Index4FirstDeathController.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
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

	SetActorsHiddenByTag(MonsterRevealTag, true);

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
	if (!IsValidPlayerActor(OtherActor))
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

	ShowSystemText(TEXT("ESCAPE ATTEMPT DETECTED"));
	SetActorsHiddenByTag(FakeExitTag, false);

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
	ShowSystemText(TEXT("REGISTRATION INCOMPLETE"));
	SetActorsHiddenByTag(MonsterRevealTag, false);
	PlaySound2DIfValid(ChaseSound);

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

	PlaySound2DIfValid(DeathSound);

	ShowSystemText(TEXT("SUBJECT TERMINATED"));
	SetActorsHiddenByTag(MonsterRevealTag, true);

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

	ShowSystemText(TEXT("DEATH IS NOT AN EXIT"), 6.0f);

	DebugMessage(TEXT("INDEX-4 subject restored"), FColor::Green, 5.0f);
}

void AIndex4FirstDeathController::ShowSystemText(const FString& Message, float AutoHideAfter)
{
	TArray<AActor*> TextActors;

	if (!SystemTextTag.IsNone())
	{
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), SystemTextTag, TextActors);
	}
	else
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMercySystemTextActor::StaticClass(), TextActors);
	}

	for (AActor* Actor : TextActors)
	{
		AMercySystemTextActor* TextActor = Cast<AMercySystemTextActor>(Actor);

		if (TextActor)
		{
			TextActor->ShowTypewriterMessage(Message, 0.035f, AutoHideAfter);
		}
	}
}

void AIndex4FirstDeathController::SetActorsHiddenByTag(FName Tag, bool bShouldHide)
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
			Actor->SetActorHiddenInGame(bShouldHide);
			Actor->SetActorEnableCollision(!bShouldHide);
		}
	}
}

void AIndex4FirstDeathController::PlaySound2DIfValid(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound);
	}
}

bool AIndex4FirstDeathController::IsValidPlayerActor(AActor* OtherActor) const
{
	if (!OtherActor)
	{
		return false;
	}

	return OtherActor->IsA<APawn>() || OtherActor->ActorHasTag(TEXT("Player"));
}

void AIndex4FirstDeathController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}