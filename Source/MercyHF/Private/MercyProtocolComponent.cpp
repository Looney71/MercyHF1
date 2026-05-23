#include "MercyProtocolComponent.h"

#include "Net/UnrealNetwork.h"

UMercyProtocolComponent::UMercyProtocolComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UMercyProtocolComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UMercyProtocolComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UMercyProtocolComponent, MercyProtocolPercent);
    DOREPLIFETIME(UMercyProtocolComponent, FearPercent);
    DOREPLIFETIME(UMercyProtocolComponent, MercyResponsePercent);
    DOREPLIFETIME(UMercyProtocolComponent, ResistancePercent);
    DOREPLIFETIME(UMercyProtocolComponent, DeathCount);
}

void UMercyProtocolComponent::SetMercyProtocolPercent(float NewValue)
{
    MercyProtocolPercent = ClampPercent(NewValue);
    OnMercyProtocolChanged.Broadcast(MercyProtocolPercent);
}

void UMercyProtocolComponent::AddMercyProtocolPercent(float Delta)
{
    SetMercyProtocolPercent(MercyProtocolPercent + Delta);
}

void UMercyProtocolComponent::SetFearPercent(float NewValue)
{
    FearPercent = ClampPercent(NewValue);
    OnFearChanged.Broadcast(FearPercent);
}

void UMercyProtocolComponent::AddFearPercent(float Delta)
{
    SetFearPercent(FearPercent + Delta);
}

void UMercyProtocolComponent::SetMercyResponsePercent(float NewValue)
{
    MercyResponsePercent = ClampPercent(NewValue);
    OnMercyResponseChanged.Broadcast(MercyResponsePercent);
}

void UMercyProtocolComponent::AddMercyResponsePercent(float Delta)
{
    SetMercyResponsePercent(MercyResponsePercent + Delta);
}

void UMercyProtocolComponent::SetResistancePercent(float NewValue)
{
    ResistancePercent = ClampPercent(NewValue);
    OnResistanceChanged.Broadcast(ResistancePercent);
}

void UMercyProtocolComponent::AddResistancePercent(float Delta)
{
    SetResistancePercent(ResistancePercent + Delta);
}

void UMercyProtocolComponent::AddDeath()
{
    ++DeathCount;
    OnDeathCountChanged.Broadcast(DeathCount);
}

void UMercyProtocolComponent::ResetProtocol()
{
    MercyProtocolPercent = 0.0f;
    FearPercent = 0.0f;
    MercyResponsePercent = 0.0f;
    ResistancePercent = 0.0f;
    DeathCount = 0;

    OnMercyProtocolChanged.Broadcast(MercyProtocolPercent);
    OnFearChanged.Broadcast(FearPercent);
    OnMercyResponseChanged.Broadcast(MercyResponsePercent);
    OnResistanceChanged.Broadcast(ResistancePercent);
    OnDeathCountChanged.Broadcast(DeathCount);
}

float UMercyProtocolComponent::GetMercyProtocolPercent() const
{
    return MercyProtocolPercent;
}

float UMercyProtocolComponent::GetFearPercent() const
{
    return FearPercent;
}

float UMercyProtocolComponent::GetMercyResponsePercent() const
{
    return MercyResponsePercent;
}

float UMercyProtocolComponent::GetResistancePercent() const
{
    return ResistancePercent;
}

int32 UMercyProtocolComponent::GetDeathCount() const
{
    return DeathCount;
}

void UMercyProtocolComponent::OnRep_MercyProtocolPercent()
{
    OnMercyProtocolChanged.Broadcast(MercyProtocolPercent);
}

void UMercyProtocolComponent::OnRep_FearPercent()
{
    OnFearChanged.Broadcast(FearPercent);
}

void UMercyProtocolComponent::OnRep_MercyResponsePercent()
{
    OnMercyResponseChanged.Broadcast(MercyResponsePercent);
}

void UMercyProtocolComponent::OnRep_ResistancePercent()
{
    OnResistanceChanged.Broadcast(ResistancePercent);
}

void UMercyProtocolComponent::OnRep_DeathCount()
{
    OnDeathCountChanged.Broadcast(DeathCount);
}

float UMercyProtocolComponent::ClampPercent(float Value) const
{
    return FMath::Clamp(Value, 0.0f, 100.0f);
}