#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MercyProtocolComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyFloatValueChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyDeathCountChanged, int32, NewDeathCount);

UCLASS(ClassGroup=(MercyMaze), meta=(BlueprintSpawnableComponent))
class MERCYHF_API UMercyProtocolComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMercyProtocolComponent();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_MercyProtocolPercent, Category="Mercy Maze|Protocol", meta=(AllowPrivateAccess="true"))
    float MercyProtocolPercent = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_FearPercent, Category="Mercy Maze|Protocol", meta=(AllowPrivateAccess="true"))
    float FearPercent = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_MercyResponsePercent, Category="Mercy Maze|Protocol", meta=(AllowPrivateAccess="true"))
    float MercyResponsePercent = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ResistancePercent, Category="Mercy Maze|Protocol", meta=(AllowPrivateAccess="true"))
    float ResistancePercent = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_DeathCount, Category="Mercy Maze|Protocol", meta=(AllowPrivateAccess="true"))
    int32 DeathCount = 0;

public:
    UPROPERTY(BlueprintAssignable, Category="Mercy Maze|Protocol")
    FMercyFloatValueChanged OnMercyProtocolChanged;

    UPROPERTY(BlueprintAssignable, Category="Mercy Maze|Protocol")
    FMercyFloatValueChanged OnFearChanged;

    UPROPERTY(BlueprintAssignable, Category="Mercy Maze|Protocol")
    FMercyFloatValueChanged OnMercyResponseChanged;

    UPROPERTY(BlueprintAssignable, Category="Mercy Maze|Protocol")
    FMercyFloatValueChanged OnResistanceChanged;

    UPROPERTY(BlueprintAssignable, Category="Mercy Maze|Protocol")
    FMercyDeathCountChanged OnDeathCountChanged;

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void SetMercyProtocolPercent(float NewValue);

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void AddMercyProtocolPercent(float Delta);

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void SetFearPercent(float NewValue);

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void AddFearPercent(float Delta);

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void SetMercyResponsePercent(float NewValue);

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void AddMercyResponsePercent(float Delta);

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void SetResistancePercent(float NewValue);

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void AddResistancePercent(float Delta);

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void AddDeath();

    UFUNCTION(BlueprintCallable, Category="Mercy Maze|Protocol")
    void ResetProtocol();

    UFUNCTION(BlueprintPure, Category="Mercy Maze|Protocol")
    float GetMercyProtocolPercent() const;

    UFUNCTION(BlueprintPure, Category="Mercy Maze|Protocol")
    float GetFearPercent() const;

    UFUNCTION(BlueprintPure, Category="Mercy Maze|Protocol")
    float GetMercyResponsePercent() const;

    UFUNCTION(BlueprintPure, Category="Mercy Maze|Protocol")
    float GetResistancePercent() const;

    UFUNCTION(BlueprintPure, Category="Mercy Maze|Protocol")
    int32 GetDeathCount() const;

private:
    UFUNCTION()
    void OnRep_MercyProtocolPercent();

    UFUNCTION()
    void OnRep_FearPercent();

    UFUNCTION()
    void OnRep_MercyResponsePercent();

    UFUNCTION()
    void OnRep_ResistancePercent();

    UFUNCTION()
    void OnRep_DeathCount();

    float ClampPercent(float Value) const;
};