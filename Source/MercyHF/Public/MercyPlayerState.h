#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MercyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyPlayerIntChanged, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyPlayerFloatChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyPlayerBoolChanged, bool, bNewValue);

UCLASS()
class MERCYHF_API AMercyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMercyPlayerState();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerFearPercent, Category = "Mercy Maze|Player", meta = (AllowPrivateAccess = "true"))
	float PlayerFearPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerMercyPercent, Category = "Mercy Maze|Player", meta = (AllowPrivateAccess = "true"))
	float PlayerMercyPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerResistancePercent, Category = "Mercy Maze|Player", meta = (AllowPrivateAccess = "true"))
	float PlayerResistancePercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerDeathCount, Category = "Mercy Maze|Player", meta = (AllowPrivateAccess = "true"))
	int32 PlayerDeathCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_bPlayerReady, Category = "Mercy Maze|Player", meta = (AllowPrivateAccess = "true"))
	bool bPlayerReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_bPlayerMarked, Category = "Mercy Maze|Player", meta = (AllowPrivateAccess = "true"))
	bool bPlayerMarked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentIndexName, Category = "Mercy Maze|Player", meta = (AllowPrivateAccess = "true"))
	FName CurrentIndexName = TEXT("INDEX_0_ENTRY");

public:
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Player")
	FMercyPlayerFloatChanged OnPlayerFearChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Player")
	FMercyPlayerFloatChanged OnPlayerMercyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Player")
	FMercyPlayerFloatChanged OnPlayerResistanceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Player")
	FMercyPlayerIntChanged OnPlayerDeathCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Player")
	FMercyPlayerBoolChanged OnPlayerReadyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Player")
	FMercyPlayerBoolChanged OnPlayerMarkedChanged;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Player")
	void SetPlayerReady(bool bNewReady);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Player")
	void SetPlayerMarked(bool bNewMarked);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Player")
	void SetCurrentIndexName(FName NewIndexName);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Player")
	void AddFear(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Player")
	void AddMercy(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Player")
	void AddResistance(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Player")
	void AddDeath();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Player")
	void ResetPlayerIndexStats();

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Player")
	float GetPlayerFearPercent() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Player")
	float GetPlayerMercyPercent() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Player")
	float GetPlayerResistancePercent() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Player")
	int32 GetPlayerDeathCount() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Player")
	bool IsPlayerReady() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Player")
	bool IsPlayerMarked() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Player")
	FName GetCurrentIndexName() const;

private:
	UFUNCTION()
	void OnRep_PlayerFearPercent();

	UFUNCTION()
	void OnRep_PlayerMercyPercent();

	UFUNCTION()
	void OnRep_PlayerResistancePercent();

	UFUNCTION()
	void OnRep_PlayerDeathCount();

	UFUNCTION()
	void OnRep_bPlayerReady();

	UFUNCTION()
	void OnRep_bPlayerMarked();

	UFUNCTION()
	void OnRep_CurrentIndexName();

	float ClampPercent(float Value) const;
};