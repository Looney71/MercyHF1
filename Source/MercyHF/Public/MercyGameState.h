#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MercyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyIndexChanged, FName, NewIndexName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyGroupFloatChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMercyGroupIntChanged, int32, NewValue);

UCLASS()
class MERCYHF_API AMercyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMercyGameState();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentIndexName, Category = "Mercy Maze|Game State", meta = (AllowPrivateAccess = "true"))
	FName CurrentIndexName = TEXT("INDEX_0_ENTRY");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GroupMercyProtocolPercent, Category = "Mercy Maze|Game State", meta = (AllowPrivateAccess = "true"))
	float GroupMercyProtocolPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GroupFearPercent, Category = "Mercy Maze|Game State", meta = (AllowPrivateAccess = "true"))
	float GroupFearPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GroupDeathCount, Category = "Mercy Maze|Game State", meta = (AllowPrivateAccess = "true"))
	int32 GroupDeathCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CompletedIndexes, Category = "Mercy Maze|Game State", meta = (AllowPrivateAccess = "true"))
	TArray<FName> CompletedIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_UnlockedIndexes, Category = "Mercy Maze|Game State", meta = (AllowPrivateAccess = "true"))
	TArray<FName> UnlockedIndexes;

public:
	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Game State")
	FMercyIndexChanged OnCurrentIndexChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Game State")
	FMercyGroupFloatChanged OnGroupMercyProtocolChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Game State")
	FMercyGroupFloatChanged OnGroupFearChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mercy Maze|Game State")
	FMercyGroupIntChanged OnGroupDeathCountChanged;

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Game State")
	void SetCurrentIndexName(FName NewIndexName);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Game State")
	void AddGroupMercyProtocol(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Game State")
	void AddGroupFear(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Game State")
	void AddGroupDeath();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Game State")
	void MarkIndexUnlocked(FName IndexName);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|Game State")
	void MarkIndexCompleted(FName IndexName);

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Game State")
	FName GetCurrentIndexName() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Game State")
	float GetGroupMercyProtocolPercent() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Game State")
	float GetGroupFearPercent() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Game State")
	int32 GetGroupDeathCount() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Game State")
	bool IsIndexUnlocked(FName IndexName) const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Game State")
	bool IsIndexCompleted(FName IndexName) const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Game State")
	TArray<FName> GetUnlockedIndexes() const;

	UFUNCTION(BlueprintPure, Category = "Mercy Maze|Game State")
	TArray<FName> GetCompletedIndexes() const;

private:
	UFUNCTION()
	void OnRep_CurrentIndexName();

	UFUNCTION()
	void OnRep_GroupMercyProtocolPercent();

	UFUNCTION()
	void OnRep_GroupFearPercent();

	UFUNCTION()
	void OnRep_GroupDeathCount();

	UFUNCTION()
	void OnRep_CompletedIndexes();

	UFUNCTION()
	void OnRep_UnlockedIndexes();

	float ClampPercent(float Value) const;
};