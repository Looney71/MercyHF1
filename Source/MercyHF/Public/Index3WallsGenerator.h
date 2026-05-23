#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Index3WallsGenerator.generated.h"

class UStaticMesh;
class UMaterialInterface;

UCLASS()
class MERCYHF_API AIndex3WallsGenerator : public AActor
{
	GENERATED_BODY()

public:
	AIndex3WallsGenerator();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation")
	bool bGenerateOnBeginPlay = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation")
	bool bClearPreviousRuntimeGeneration = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation")
	int32 RandomSeed = 1307;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation", meta = (ClampMin = "100.0"))
	float TileSize = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation", meta = (ClampMin = "100.0"))
	float WallHeight = 340.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation", meta = (ClampMin = "5.0"))
	float WallThickness = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation", meta = (ClampMin = "5.0"))
	float FloorThickness = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation", meta = (ClampMin = "5.0"))
	float CeilingThickness = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation")
	bool bGenerateCeiling = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation")
	bool bGenerateOuterWalls = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation")
	bool bCenterMazeOnActor = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Generation")
	TArray<FString> LayoutRows;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Meshes")
	UStaticMesh* CubeMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Materials")
	UMaterialInterface* WallMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Materials")
	UMaterialInterface* FloorMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Materials")
	UMaterialInterface* CeilingMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Lighting")
	float RedLightIntensity = 700.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Lighting")
	float RedLightRadius = 950.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Lighting")
	float RedLightHeight = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Text")
	float SystemTextWorldSize = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Text")
	FColor SystemTextColor = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = true;

	UPROPERTY()
	TArray<AActor*> RuntimeGeneratedActors;

	void EnsureDefaultLayout();
	void GenerateWallsIndex();
	void ClearRuntimeGeneratedActors();

	int32 GetMaxColumnCount() const;
	FVector GetCellWorldLocation(int32 RowIndex, int32 ColumnIndex, int32 RowCount, int32 ColumnCount) const;

	bool IsWallCell(TCHAR CellSymbol) const;
	bool IsFloorCell(TCHAR CellSymbol) const;

	void SpawnFloorTile(const FVector& CellCenter);
	void SpawnCeilingTile(const FVector& CellCenter);
	void SpawnWallTile(const FVector& CellCenter);
	void SpawnRedLight(const FVector& CellCenter);
	void SpawnSystemText(const FString& Message, const FVector& CellCenter, float YawDegrees);

	AActor* SpawnCubeActor(
		const FString& ActorLabel,
		const FVector& Location,
		const FVector& Scale,
		UMaterialInterface* Material
	);

	void RegisterGeneratedActor(AActor* Actor);
	void ApplyGeneratedActorTags(AActor* Actor, const FName& SpecificTag) const;

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};