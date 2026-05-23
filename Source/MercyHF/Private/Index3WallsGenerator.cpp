#include "Index3WallsGenerator.h"

#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "Engine/PointLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextRenderActor.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AIndex3WallsGenerator::AIndex3WallsGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));

	if (CubeMeshFinder.Succeeded())
	{
		CubeMesh = CubeMeshFinder.Object;
	}

	EnsureDefaultLayout();
}

void AIndex3WallsGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (bGenerateOnBeginPlay)
	{
		GenerateWallsIndex();
	}
}

void AIndex3WallsGenerator::EnsureDefaultLayout()
{
	if (LayoutRows.Num() > 0)
	{
		return;
	}

	LayoutRows.Add(TEXT("#############"));
	LayoutRows.Add(TEXT("#S..#.......#"));
	LayoutRows.Add(TEXT("#.#.#.#####.#"));
	LayoutRows.Add(TEXT("#.#...#H..#.#"));
	LayoutRows.Add(TEXT("#.#####.#.#.#"));
	LayoutRows.Add(TEXT("#.....#.#...#"));
	LayoutRows.Add(TEXT("###.#.#.###.#"));
	LayoutRows.Add(TEXT("#...#...#E..#"));
	LayoutRows.Add(TEXT("#.#######.#.#"));
	LayoutRows.Add(TEXT("#.........#.#"));
	LayoutRows.Add(TEXT("#############"));
}

void AIndex3WallsGenerator::GenerateWallsIndex()
{
	if (!GetWorld())
	{
		return;
	}

	if (!CubeMesh)
	{
		DebugMessage(TEXT("Index3WallsGenerator: CubeMesh is missing."), FColor::Red, 6.0f);
		return;
	}

	EnsureDefaultLayout();

	if (bClearPreviousRuntimeGeneration)
	{
		ClearRuntimeGeneratedActors();
	}

	const int32 RowCount = LayoutRows.Num();
	const int32 ColumnCount = GetMaxColumnCount();

	if (RowCount <= 0 || ColumnCount <= 0)
	{
		DebugMessage(TEXT("Index3WallsGenerator: Layout is empty."), FColor::Red, 6.0f);
		return;
	}

	int32 FloorCount = 0;
	int32 CeilingCount = 0;
	int32 WallCount = 0;
	int32 LightCount = 0;
	int32 TextCount = 0;

	for (int32 RowIndex = 0; RowIndex < RowCount; ++RowIndex)
	{
		const FString& RowText = LayoutRows[RowIndex];

		for (int32 ColumnIndex = 0; ColumnIndex < ColumnCount; ++ColumnIndex)
		{
			const TCHAR CellSymbol = ColumnIndex < RowText.Len() ? RowText[ColumnIndex] : TCHAR('#');
			const FVector CellCenter = GetCellWorldLocation(RowIndex, ColumnIndex, RowCount, ColumnCount);

			if (IsWallCell(CellSymbol))
			{
				const bool bIsInnerCell =
					RowIndex > 0 &&
					RowIndex < RowCount - 1 &&
					ColumnIndex > 0 &&
					ColumnIndex < ColumnCount - 1;

				if (bGenerateOuterWalls || bIsInnerCell)
				{
					SpawnWallTile(CellCenter);
					++WallCount;
				}

				continue;
			}

			if (IsFloorCell(CellSymbol))
			{
				SpawnFloorTile(CellCenter);
				++FloorCount;

				if (bGenerateCeiling)
				{
					SpawnCeilingTile(CellCenter);
					++CeilingCount;
				}
			}

			if (CellSymbol == TCHAR('S'))
			{
				SpawnSystemText(TEXT("SUBJECTS DETECTED"), CellCenter, 180.0f);
				++TextCount;
			}
			else if (CellSymbol == TCHAR('H'))
			{
				SpawnRedLight(CellCenter);
				SpawnSystemText(TEXT("HELP"), CellCenter, 180.0f);
				++LightCount;
				++TextCount;
			}
			else if (CellSymbol == TCHAR('E'))
			{
				SpawnRedLight(CellCenter);
				SpawnSystemText(TEXT("EXIT PROPERTY: ABSENT"), CellCenter, 180.0f);
				++LightCount;
				++TextCount;
			}
			else if (CellSymbol == TCHAR('V'))
			{
				SpawnSystemText(TEXT("DO NOT FOLLOW VOICES"), CellCenter, 180.0f);
				++TextCount;
			}
		}
	}

	DebugMessage(
		FString::Printf(
			TEXT("INDEX-3 WALLS generated | Floors: %d | Ceilings: %d | Walls: %d | Lights: %d | Texts: %d"),
			FloorCount,
			CeilingCount,
			WallCount,
			LightCount,
			TextCount
		),
		FColor::Green,
		10.0f
	);
}

void AIndex3WallsGenerator::ClearRuntimeGeneratedActors()
{
	for (AActor* Actor : RuntimeGeneratedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}

	RuntimeGeneratedActors.Empty();
}

int32 AIndex3WallsGenerator::GetMaxColumnCount() const
{
	int32 MaxColumns = 0;

	for (const FString& RowText : LayoutRows)
	{
		MaxColumns = FMath::Max(MaxColumns, RowText.Len());
	}

	return MaxColumns;
}

FVector AIndex3WallsGenerator::GetCellWorldLocation(int32 RowIndex, int32 ColumnIndex, int32 RowCount, int32 ColumnCount) const
{
	const float OriginOffsetX = bCenterMazeOnActor ? -static_cast<float>(ColumnCount - 1) * TileSize * 0.5f : 0.0f;
	const float OriginOffsetY = bCenterMazeOnActor ? -static_cast<float>(RowCount - 1) * TileSize * 0.5f : 0.0f;

	const FVector LocalLocation(
		OriginOffsetX + static_cast<float>(ColumnIndex) * TileSize,
		OriginOffsetY + static_cast<float>(RowIndex) * TileSize,
		0.0f
	);

	return GetActorTransform().TransformPosition(LocalLocation);
}

bool AIndex3WallsGenerator::IsWallCell(TCHAR CellSymbol) const
{
	return CellSymbol == TCHAR('#');
}

bool AIndex3WallsGenerator::IsFloorCell(TCHAR CellSymbol) const
{
	return CellSymbol == TCHAR('.')
		|| CellSymbol == TCHAR('S')
		|| CellSymbol == TCHAR('H')
		|| CellSymbol == TCHAR('E')
		|| CellSymbol == TCHAR('V');
}

void AIndex3WallsGenerator::SpawnFloorTile(const FVector& CellCenter)
{
	const FVector Location(CellCenter.X, CellCenter.Y, CellCenter.Z - FloorThickness * 0.5f);
	const FVector Scale(TileSize / 100.0f, TileSize / 100.0f, FloorThickness / 100.0f);

	SpawnCubeActor(TEXT("MM_Index3_Floor"), Location, Scale, FloorMaterial);
}

void AIndex3WallsGenerator::SpawnCeilingTile(const FVector& CellCenter)
{
	const FVector Location(CellCenter.X, CellCenter.Y, CellCenter.Z + WallHeight + CeilingThickness * 0.5f);
	const FVector Scale(TileSize / 100.0f, TileSize / 100.0f, CeilingThickness / 100.0f);

	SpawnCubeActor(TEXT("MM_Index3_Ceiling"), Location, Scale, CeilingMaterial);
}

void AIndex3WallsGenerator::SpawnWallTile(const FVector& CellCenter)
{
	const FVector Location(CellCenter.X, CellCenter.Y, CellCenter.Z + WallHeight * 0.5f);
	const FVector Scale(TileSize / 100.0f, TileSize / 100.0f, WallHeight / 100.0f);

	SpawnCubeActor(TEXT("MM_Index3_Wall"), Location, Scale, WallMaterial);
}

void AIndex3WallsGenerator::SpawnRedLight(const FVector& CellCenter)
{
	if (!GetWorld())
	{
		return;
	}

	const FVector Location(CellCenter.X, CellCenter.Y, CellCenter.Z + RedLightHeight);

	APointLight* NewLightActor = GetWorld()->SpawnActor<APointLight>(
		APointLight::StaticClass(),
		Location,
		FRotator::ZeroRotator
	);

	if (!NewLightActor)
	{
		return;
	}

#if WITH_EDITOR
	NewLightActor->SetActorLabel(TEXT("MM_Index3_RedLight"));
#endif

	UPointLightComponent* PointLightComponent = Cast<UPointLightComponent>(NewLightActor->GetLightComponent());

	if (PointLightComponent)
	{
		PointLightComponent->SetMobility(EComponentMobility::Movable);
		PointLightComponent->SetIntensity(RedLightIntensity);
		PointLightComponent->SetAttenuationRadius(RedLightRadius);
		PointLightComponent->SetLightColor(FLinearColor(1.0f, 0.0f, 0.0f));
		PointLightComponent->SetCastShadows(true);
	}

	ApplyGeneratedActorTags(NewLightActor, TEXT("INDEX3_GENERATED_LIGHT"));
	RegisterGeneratedActor(NewLightActor);
}

void AIndex3WallsGenerator::SpawnSystemText(const FString& Message, const FVector& CellCenter, float YawDegrees)
{
	if (!GetWorld())
	{
		return;
	}

	const FVector Location(CellCenter.X, CellCenter.Y, CellCenter.Z + WallHeight * 0.55f);
	const FRotator Rotation(0.0f, YawDegrees, 0.0f);

	ATextRenderActor* NewTextActor = GetWorld()->SpawnActor<ATextRenderActor>(
		ATextRenderActor::StaticClass(),
		Location,
		Rotation
	);

	if (!NewTextActor)
	{
		return;
	}

#if WITH_EDITOR
	NewTextActor->SetActorLabel(TEXT("MM_Index3_SystemText"));
#endif

	UTextRenderComponent* TextComponent = NewTextActor->GetTextRender();

	if (TextComponent)
	{
		TextComponent->SetMobility(EComponentMobility::Movable);
		TextComponent->SetText(FText::FromString(Message));
		TextComponent->SetWorldSize(SystemTextWorldSize);
		TextComponent->SetHorizontalAlignment(EHTA_Center);
		TextComponent->SetVerticalAlignment(EVRTA_TextCenter);
		TextComponent->SetTextRenderColor(SystemTextColor);
	}

	ApplyGeneratedActorTags(NewTextActor, TEXT("INDEX3_GENERATED_TEXT"));
	RegisterGeneratedActor(NewTextActor);
}

AActor* AIndex3WallsGenerator::SpawnCubeActor(
	const FString& NewActorLabel,
	const FVector& Location,
	const FVector& Scale,
	UMaterialInterface* Material
)
{
	if (!GetWorld() || !CubeMesh)
	{
		return nullptr;
	}

	AStaticMeshActor* NewMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(),
		Location,
		FRotator::ZeroRotator
	);

	if (!NewMeshActor)
	{
		return nullptr;
	}

#if WITH_EDITOR
	NewMeshActor->SetActorLabel(NewActorLabel);
#endif

	UStaticMeshComponent* MeshComponent = NewMeshActor->GetStaticMeshComponent();

	if (MeshComponent)
	{
		MeshComponent->SetMobility(EComponentMobility::Movable);
		MeshComponent->SetStaticMesh(CubeMesh);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));

		if (Material)
		{
			MeshComponent->SetMaterial(0, Material);
		}
	}

	NewMeshActor->SetActorScale3D(Scale);

	ApplyGeneratedActorTags(NewMeshActor, TEXT("INDEX3_GENERATED_MESH"));
	RegisterGeneratedActor(NewMeshActor);

	return NewMeshActor;
}

void AIndex3WallsGenerator::RegisterGeneratedActor(AActor* Actor)
{
	if (Actor)
	{
		RuntimeGeneratedActors.Add(Actor);
	}
}

void AIndex3WallsGenerator::ApplyGeneratedActorTags(AActor* Actor, const FName& SpecificTag) const
{
	if (!Actor)
	{
		return;
	}

	Actor->Tags.AddUnique(TEXT("MERCY_MAZE_GENERATED"));
	Actor->Tags.AddUnique(TEXT("INDEX3_WALLS"));

	if (!SpecificTag.IsNone())
	{
		Actor->Tags.AddUnique(SpecificTag);
	}
}

void AIndex3WallsGenerator::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}