#include "MercyTriggerVolume.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MercyHelpers.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AMercyTriggerVolume::AMercyTriggerVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(SceneRoot);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMercyTriggerVolume::BeginPlay()
{
    Super::BeginPlay();

    if (TriggerBox)
    {
        TriggerBox->SetBoxExtent(TriggerBoxExtent);
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMercyTriggerVolume::HandleTriggerBeginOverlap);
    }

    DebugMessage(TEXT("MercyTriggerVolume ready"), FColor::Cyan, 3.0f);
}

void AMercyTriggerVolume::HandleTriggerBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!bEnabled || !bTriggerOnPlayerOverlap)
    {
        return;
    }

    if (!IsValidPlayerActor(OtherActor))
    {
        return;
    }

    ActivateTrigger();
}

void AMercyTriggerVolume::ActivateTrigger()
{
    if (!bEnabled)
    {
        return;
    }

    if (bTriggerOnlyOnce && bHasTriggered)
    {
        return;
    }

    bHasTriggered = true;

    if (ActivationDelay > 0.0f)
    {
        GetWorldTimerManager().SetTimer(
            ActivationTimerHandle,
            this,
            &AMercyTriggerVolume::ExecuteActivation,
            ActivationDelay,
            false
        );
    }
    else
    {
        ExecuteActivation();
    }
}

void AMercyTriggerVolume::ExecuteActivation()
{
    if (SoundToPlay)
    {
        if (bPlaySound2D)
        {
            UGameplayStatics::PlaySound2D(this, SoundToPlay, VolumeMultiplier, PitchMultiplier);
        }
        else
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                SoundToPlay,
                GetActorLocation(),
                GetActorRotation(),
                VolumeMultiplier,
                PitchMultiplier
            );
        }
    }

    ApplyActorVisibilityList(ActorsToShow, false);
    ApplyActorVisibilityList(ActorsToHide, true);
    ApplyActorCollisionList(ActorsToEnableCollision, true);
    ApplyActorCollisionList(ActorsToDisableCollision, false);

    ApplyActorVisibilityByTag(ActorsToShowTag, false);
    ApplyActorVisibilityByTag(ActorsToHideTag, true);
    ApplyActorCollisionByTag(ActorsToEnableCollisionTag, true);
    ApplyActorCollisionByTag(ActorsToDisableCollisionTag, false);

    OnMercyTriggerActivated.Broadcast(this);

    DebugMessage(TEXT("MercyTriggerVolume activated"), FColor::Green, 4.0f);
}

void AMercyTriggerVolume::ResetTrigger()
{
    bHasTriggered = false;
    GetWorldTimerManager().ClearTimer(ActivationTimerHandle);

    DebugMessage(TEXT("MercyTriggerVolume reset"), FColor::Cyan, 3.0f);
}

void AMercyTriggerVolume::EnableTrigger()
{
    bEnabled = true;
    DebugMessage(TEXT("MercyTriggerVolume enabled"), FColor::Green, 3.0f);
}

void AMercyTriggerVolume::DisableTrigger()
{
    bEnabled = false;
    DebugMessage(TEXT("MercyTriggerVolume disabled"), FColor::Yellow, 3.0f);
}

bool AMercyTriggerVolume::HasTriggered() const
{
    return bHasTriggered;
}

void AMercyTriggerVolume::ApplyActorVisibilityByTag(FName Tag, bool bShouldHide)
{
    if (Tag.IsNone())
    {
        return;
    }

    // Use MercyHelpers for consistent collision-aware hiding
    UMercyHelpers::SetActorsHiddenByTag(this, Tag, bShouldHide, true);

    if (bShowDebugMessages)
    {
        const FString Action = bShouldHide ? TEXT("Hiding") : TEXT("Showing");
        DebugMessage(FString::Printf(TEXT("MercyTrigger: %s actors with tag '%s'"), *Action, *Tag.ToString()), FColor::Orange, 3.0f);
    }
}

void AMercyTriggerVolume::ApplyActorVisibilityList(const TArray<AActor*>& Actors, bool bShouldHide)
{
    if (Actors.Num() == 0)
    {
        return;
    }

    const FString Action = bShouldHide ? TEXT("Hiding") : TEXT("Showing");
    if (bShowDebugMessages)
    {
        DebugMessage(FString::Printf(TEXT("MercyTrigger: %s %d specific actors"), *Action, Actors.Num()), FColor::Orange, 3.0f);
    }

    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            Actor->SetActorHiddenInGame(bShouldHide);
            Actor->SetActorEnableCollision(!bShouldHide);
        }
    }
}

void AMercyTriggerVolume::ApplyActorCollisionByTag(FName Tag, bool bEnableCollision)
{
    if (Tag.IsNone())
    {
        return;
    }

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

    ApplyActorCollisionList(FoundActors, bEnableCollision);
}

void AMercyTriggerVolume::ApplyActorCollisionList(const TArray<AActor*>& Actors, bool bEnableCollision)
{
    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            Actor->SetActorEnableCollision(bEnableCollision);
        }
    }
}

bool AMercyTriggerVolume::IsValidPlayerActor(AActor* OtherActor) const
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

void AMercyTriggerVolume::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

    if (bShowDebugMessages && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
    }
}