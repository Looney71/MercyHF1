#include "Index0EntryController.h"

#include "Components/LightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "MercySystemTextActor.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "MercyHelpers.h"

AIndex0EntryController::AIndex0EntryController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AIndex0EntryController::BeginPlay()
{
	Super::BeginPlay();

	DebugMessage(TEXT("=== INDEX-0 ENTRY CONTROLLER STARTING ==="), FColor::Green, 6.0f);
	DebugMessage(FString::Printf(TEXT("Controller placed at: %s"), *GetActorLocation().ToString()), FColor::Green, 4.0f);

	CacheEntryActors();

	// Initial setup with detailed logging
	DebugMessage(TEXT("Setting initial actor states..."), FColor::Blue, 3.0f);
	SetActorsHidden(WarningTextActors, false); // Keep visible but unlit/dark initially
	SetActorsHidden(PathRevealActors, true);

	SetEntryLightsEnabled(MainLightActors, true);
	SetEntryLightsEnabled(RedLightActors, true);

	SetLightColor(RedLightActors, RedLightColor);
	SetLightIntensity(RedLightActors, RedLightIdleIntensity);
	SetTextForActors(WarningTextActors, TEXT("FOLLOW THE VOICES"));

	// Timer setup with detailed logging
	DebugMessage(FString::Printf(TEXT("Setting up timers: Blackout=%.1fs, Warning=%.1fs, Help=%.1fs, Path=%.1fs"),
		LightsOffDelay, WarningTextDelay, HelpVoiceDelay, PathRevealDelay), FColor::Blue, 5.0f);

	GetWorldTimerManager().SetTimer(
		LightsOffTimerHandle,
		this,
		&AIndex0EntryController::StartBlackout,
		LightsOffDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		WarningTextTimerHandle,
		this,
		&AIndex0EntryController::ShowWarningText,
		WarningTextDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		HelpVoiceTimerHandle,
		this,
		&AIndex0EntryController::PlayHelpVoice,
		HelpVoiceDelay,
		false
	);

	GetWorldTimerManager().SetTimer(
		PathRevealTimerHandle,
		this,
		&AIndex0EntryController::RevealPathForward,
		PathRevealDelay,
		false
	);

	DebugMessage(TEXT("=== INDEX-0 ENTRY CONTROLLER READY ==="), FColor::Green, 6.0f);
}

void AIndex0EntryController::CacheEntryActors()
{
	MainLightActors.Empty();
	RedLightActors.Empty();
	WarningTextActors.Empty();
	PathRevealActors.Empty();
	CachedMaterials.Empty();

	DebugMessage(TEXT("=== INDEX-0 ACTOR CACHING START ==="), FColor::White, 8.0f);

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	DebugMessage(FString::Printf(TEXT("Total actors in level: %d"), AllActors.Num()), FColor::White, 6.0f);

	// Track what we find for detailed reporting
	int32 MainLightsByTag = 0, MainLightsByName = 0;
	int32 RedLightsByTag = 0, RedLightsByName = 0;
	int32 WarningTextsByTag = 0, WarningTextsByName = 0;
	int32 PathRevealsByTag = 0, PathRevealsByName = 0;

	for (AActor* Actor : AllActors)
	{
		if (!Actor)
		{
			continue;
		}

		// Check main lights with detailed tracking
		if (ActorMatches(Actor, MainLightTag, TEXT("L_Entry_Main")))
		{
			MainLightActors.Add(Actor);
			if (Actor->ActorHasTag(MainLightTag))
			{
				MainLightsByTag++;
				DebugMessage(FString::Printf(TEXT("Main light found by TAG: %s"), *Actor->GetName()), FColor::Green, 6.0f);
			}
			else
			{
				MainLightsByName++;
				DebugMessage(FString::Printf(TEXT("Main light found by NAME: %s"), *Actor->GetName()), FColor::Yellow, 6.0f);
			}
		}

		// Check red lights with detailed tracking
		if (ActorMatches(Actor, RedLightTag, TEXT("L_Entry_Red")))
		{
			RedLightActors.Add(Actor);
			if (Actor->ActorHasTag(RedLightTag))
			{
				RedLightsByTag++;
				DebugMessage(FString::Printf(TEXT("Red light found by TAG: %s"), *Actor->GetName()), FColor::Green, 6.0f);
			}
			else
			{
				RedLightsByName++;
				DebugMessage(FString::Printf(TEXT("Red light found by NAME: %s"), *Actor->GetName()), FColor::Yellow, 6.0f);
			}
		}

		// Check warning text with detailed tracking
		if (ActorMatches(Actor, WarningTextTag, TEXT("TXT_DoNotFollowVoices")))
		{
			WarningTextActors.Add(Actor);
			if (Actor->ActorHasTag(WarningTextTag))
			{
				WarningTextsByTag++;
				DebugMessage(FString::Printf(TEXT("Warning text found by TAG: %s"), *Actor->GetName()), FColor::Green, 6.0f);
			}
			else
			{
				WarningTextsByName++;
				DebugMessage(FString::Printf(TEXT("Warning text found by NAME: %s"), *Actor->GetName()), FColor::Yellow, 6.0f);
			}
		}

		// Check path reveal with detailed tracking
		if (ActorMatches(Actor, PathRevealTag, TEXT("Entry_Path")))
		{
			PathRevealActors.Add(Actor);
			if (Actor->ActorHasTag(PathRevealTag))
			{
				PathRevealsByTag++;
				DebugMessage(FString::Printf(TEXT("Path reveal found by TAG: %s"), *Actor->GetName()), FColor::Green, 6.0f);
			}
			else
			{
				PathRevealsByName++;
				DebugMessage(FString::Printf(TEXT("Path reveal found by NAME: %s"), *Actor->GetName()), FColor::Yellow, 6.0f);
			}
		}
	}

	// Summary report
	DebugMessage(FString::Printf(TEXT("MAIN LIGHTS: %d total (%d by tag, %d by name)"),
		MainLightActors.Num(), MainLightsByTag, MainLightsByName), FColor::Cyan, 8.0f);
	DebugMessage(FString::Printf(TEXT("RED LIGHTS: %d total (%d by tag, %d by name)"),
		RedLightActors.Num(), RedLightsByTag, RedLightsByName), FColor::Cyan, 8.0f);
	DebugMessage(FString::Printf(TEXT("WARNING TEXTS: %d total (%d by tag, %d by name)"),
		WarningTextActors.Num(), WarningTextsByTag, WarningTextsByName), FColor::Cyan, 8.0f);
	DebugMessage(FString::Printf(TEXT("PATH REVEALS: %d total (%d by tag, %d by name)"),
		PathRevealActors.Num(), PathRevealsByTag, PathRevealsByName), FColor::Cyan, 8.0f);

	// Cache materials for emissive panels/beacons
	UMaterialInterface* DarkConcrete = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/MercyMaze/Index0/M_IDX0_DarkConcrete.M_IDX0_DarkConcrete")));
	UMaterialInterface* BlackMetal = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/MercyMaze/Index0/M_IDX0_BlackMetal.M_IDX0_BlackMetal")));

	if (!DarkConcrete)
	{
		DebugMessage(TEXT("WARNING: Dark concrete material not found!"), FColor::Red, 10.0f);
	}
	if (!BlackMetal)
	{
		DebugMessage(TEXT("WARNING: Black metal material not found!"), FColor::Red, 10.0f);
	}

	auto CacheActorMaterials = [this, DarkConcrete, BlackMetal](const TArray<AActor*>& Actors)
	{
		for (AActor* Actor : Actors)
		{
			if (!Actor) continue;
			TArray<UStaticMeshComponent*> MeshComps;
			Actor->GetComponents<UStaticMeshComponent>(MeshComps);
			for (UStaticMeshComponent* MeshComp : MeshComps)
			{
				if (!MeshComp) continue;
				for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
				{
					UMaterialInterface* Mat = MeshComp->GetMaterial(i);
					if (Mat)
					{
						FString MatName = Mat->GetName();
						if (MatName.Contains(TEXT("Emissive")))
						{
							FCachedMaterialInfo Info;
							Info.MeshComponent = MeshComp;
							Info.MaterialIndex = i;
							Info.OriginalMaterial = Mat;
							if (MatName.Contains(TEXT("White")))
							{
								Info.OffMaterial = DarkConcrete;
							}
							else
							{
								Info.OffMaterial = BlackMetal;
							}
							CachedMaterials.Add(Info);
							DebugMessage(FString::Printf(TEXT("Cached emissive material for mesh %s in %s: %s (Off: %s)"), 
								*MeshComp->GetName(), *Actor->GetName(), *Mat->GetName(), Info.OffMaterial ? *Info.OffMaterial->GetName() : TEXT("None")), FColor::Green, 5.0f);
						}
					}
				}
			}
		}
	};

	CacheActorMaterials(MainLightActors);
	CacheActorMaterials(RedLightActors);

	DebugMessage(FString::Printf(TEXT("Total cached emissive materials: %d"), CachedMaterials.Num()), FColor::Cyan, 8.0f);

	// Warn about missing critical actors
	if (MainLightActors.Num() == 0)
	{
		DebugMessage(FString::Printf(TEXT("WARNING: No main lights found! Looking for tag '%s' or name containing 'L_Entry_Main'"),
			*MainLightTag.ToString()), FColor::Red, 10.0f);
	}
	if (RedLightActors.Num() == 0)
	{
		DebugMessage(FString::Printf(TEXT("WARNING: No red lights found! Looking for tag '%s' or name containing 'L_Entry_Red'"),
			*RedLightTag.ToString()), FColor::Red, 10.0f);
	}

	DebugMessage(TEXT("=== INDEX-0 ACTOR CACHING COMPLETE ==="), FColor::White, 8.0f);
}

bool AIndex0EntryController::ActorMatches(AActor* Actor, FName RequiredTag, const FString& NameContains) const
{
	if (!Actor)
	{
		return false;
	}

	if (!RequiredTag.IsNone() && Actor->ActorHasTag(RequiredTag))
	{
		return true;
	}

	const FString ActorName = Actor->GetName();

	if (ActorName.Contains(NameContains))
	{
		return true;
	}

	const FString ActorLabel = Actor->GetActorLabel();

	if (ActorLabel.Contains(NameContains))
	{
		return true;
	}

	return false;
}

void AIndex0EntryController::StartBlackout()
{
	DebugMessage(TEXT("=== BLACKOUT SEQUENCE STARTING ==="), FColor::Red, 6.0f);
	DebugMessage(FString::Printf(TEXT("Disabling %d main lights, intensifying %d red lights"),
		MainLightActors.Num(), RedLightActors.Num()), FColor::Red, 5.0f);

	SetEntryLightsEnabled(MainLightActors, false);
	SetLightColor(RedLightActors, RedLightColor);
	SetLightIntensity(RedLightActors, RedLightBlackoutIntensity);

	DebugMessage(TEXT("=== BLACKOUT SEQUENCE COMPLETE ==="), FColor::Red, 4.0f);
}

void AIndex0EntryController::ShowWarningText()
{
	DebugMessage(TEXT("ENTRY: warning text shown"), FColor::Yellow, 4.0f);

	ShowTextForActors(WarningTextActors, TEXT("DO NOT FOLLOW THE VOICES"));
}

void AIndex0EntryController::PlayHelpVoice()
{
	USoundBase* SoundToPlay = HelpVoiceSound;

	if (!SoundToPlay && !HelpSoundAssetPath.IsEmpty())
	{
		SoundToPlay = Cast<USoundBase>(
			StaticLoadObject(USoundBase::StaticClass(), nullptr, *HelpSoundAssetPath)
		);
	}

	if (SoundToPlay)
	{
		DebugMessage(TEXT("ENTRY: help voice played"), FColor::Purple, 4.0f);
		UGameplayStatics::PlaySound2D(this, SoundToPlay);
	}
	else
	{
		DebugMessage(TEXT("ENTRY: help voice sound not found yet"), FColor::Orange, 5.0f);
	}
}

void AIndex0EntryController::RevealPathForward()
{
	DebugMessage(TEXT("ENTRY: path forward revealed"), FColor::Green, 4.0f);

	SetActorsHidden(PathRevealActors, false);
	ShowTextForActors(WarningTextActors, TEXT("VOLUNTARY ENTRY CONFIRMED"));
}

void AIndex0EntryController::SetActorsHidden(const TArray<AActor*>& Actors, bool bShouldHide)
{
	const FString ActionName = bShouldHide ? TEXT("HIDING") : TEXT("SHOWING");
	DebugMessage(FString::Printf(TEXT("%s %d actors..."), *ActionName, Actors.Num()), FColor::Orange, 3.0f);

	for (AActor* Actor : Actors)
	{
		if (!Actor)
		{
			continue;
		}

		Actor->SetActorHiddenInGame(bShouldHide);
		Actor->SetActorEnableCollision(!bShouldHide);
	}
}

void AIndex0EntryController::SetEntryLightsEnabled(const TArray<AActor*>& Actors, bool bIsEnabled)
{
	DebugMessage(FString::Printf(TEXT("Setting entry lights/emissives enabled=%d for %d actors..."), bIsEnabled, Actors.Num()), FColor::Orange, 3.0f);

	for (AActor* Actor : Actors)
	{
		if (!Actor)
		{
			continue;
		}

		// Enable/disable light components
		TArray<ULightComponent*> LightComps;
		Actor->GetComponents<ULightComponent>(LightComps);
		for (ULightComponent* LightComp : LightComps)
		{
			if (LightComp)
			{
				LightComp->SetVisibility(bIsEnabled, true);
				if (!bIsEnabled)
				{
					LightComp->SetIntensity(0.0f);
				}
				DebugMessage(FString::Printf(TEXT("Light component %s in %s: set visibility to %d"), *LightComp->GetName(), *Actor->GetName(), bIsEnabled), FColor::Purple, 4.0f);
			}
		}

		// Update materials for static meshes in this actor using CachedMaterials
		TArray<UStaticMeshComponent*> MeshComps;
		Actor->GetComponents<UStaticMeshComponent>(MeshComps);
		for (UStaticMeshComponent* MeshComp : MeshComps)
		{
			if (!MeshComp) continue;
			for (const FCachedMaterialInfo& Info : CachedMaterials)
			{
				if (Info.MeshComponent == MeshComp)
				{
					UMaterialInterface* TargetMat = bIsEnabled ? Info.OriginalMaterial : Info.OffMaterial;
					if (TargetMat)
					{
						MeshComp->SetMaterial(Info.MaterialIndex, TargetMat);
						DebugMessage(FString::Printf(TEXT("Mesh %s in %s: set material index %d to %s"), *MeshComp->GetName(), *Actor->GetName(), Info.MaterialIndex, *TargetMat->GetName()), FColor::Purple, 4.0f);
					}
				}
			}
		}
	}
}

void AIndex0EntryController::SetLightIntensity(const TArray<AActor*>& Actors, float NewIntensity)
{
	DebugMessage(FString::Printf(TEXT("Setting light intensity to %.1f for %d actors..."), NewIntensity, Actors.Num()), FColor::Orange, 3.0f);

	for (AActor* Actor : Actors)
	{
		if (!Actor)
		{
			DebugMessage(TEXT("Skipping null actor for intensity change"), FColor::Red, 2.0f);
			continue;
		}

		ULightComponent* LightComponent = Actor->FindComponentByClass<ULightComponent>();

		if (LightComponent)
		{
			const float OldIntensity = LightComponent->Intensity;
			LightComponent->SetIntensity(NewIntensity);
			DebugMessage(FString::Printf(TEXT("Light %s: intensity %.1f -> %.1f"), *Actor->GetName(), OldIntensity, NewIntensity), FColor::Magenta, 4.0f);
		}
		else
		{
			DebugMessage(FString::Printf(TEXT("Actor %s: no light component found for intensity change"), *Actor->GetName()), FColor::Red, 4.0f);
		}
	}
}

void AIndex0EntryController::SetLightColor(const TArray<AActor*>& Actors, const FLinearColor& NewColor)
{
	for (AActor* Actor : Actors)
	{
		if (!Actor)
		{
			continue;
		}

		ULightComponent* LightComponent = Actor->FindComponentByClass<ULightComponent>();

		if (LightComponent)
		{
			LightComponent->SetLightColor(NewColor);
		}
	}
}

void AIndex0EntryController::SetTextForActors(const TArray<AActor*>& Actors, const FString& NewText)
{
	for (AActor* Actor : Actors)
	{
		if (!Actor)
		{
			continue;
		}

		if (AMercySystemTextActor* SystemTextActor = Cast<AMercySystemTextActor>(Actor))
		{
			SystemTextActor->SetMessageColor(FColor(40, 40, 40)); // Dark, unlit gray!
			SystemTextActor->ShowInstantMessage(NewText);
			continue;
		}

		UTextRenderComponent* TextComponent = Actor->FindComponentByClass<UTextRenderComponent>();

		if (TextComponent)
		{
			TextComponent->SetText(FText::FromString(NewText));
			TextComponent->SetTextRenderColor(FColor(40, 40, 40)); // Dark, unlit gray!
		}
	}
}

void AIndex0EntryController::ShowTextForActors(const TArray<AActor*>& Actors, const FString& NewText)
{
	for (AActor* Actor : Actors)
	{
		if (!Actor)
		{
			continue;
		}

		Actor->SetActorHiddenInGame(false);

		if (AMercySystemTextActor* SystemTextActor = Cast<AMercySystemTextActor>(Actor))
		{
			if (NewText.Contains(TEXT("VOLUNTARY")))
			{
				SystemTextActor->SetMessageColor(FColor(180, 180, 180)); // Medium gray for path confirmed
			}
			else
			{
				SystemTextActor->SetMessageColor(FColor(255, 0, 0)); // Vibrant red
			}
			SystemTextActor->ShowInstantMessage(NewText);
			continue;
		}

		UTextRenderComponent* TextComponent = Actor->FindComponentByClass<UTextRenderComponent>();

		if (TextComponent)
		{
			TextComponent->SetText(FText::FromString(NewText));
			TextComponent->SetVisibility(true, true);
			TextComponent->SetHiddenInGame(false);
			
			if (NewText.Contains(TEXT("VOLUNTARY")))
			{
				TextComponent->SetTextRenderColor(FColor(180, 180, 180)); // Medium gray
			}
			else
			{
				TextComponent->SetTextRenderColor(FColor(255, 0, 0)); // Vibrant red
			}
		}
	}
}

void AIndex0EntryController::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}
