#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MercySystemTextActor.generated.h"

class USceneComponent;
class UTextRenderComponent;

UCLASS()
class MERCYHF_API AMercySystemTextActor : public AActor
{
	GENERATED_BODY()

public:
	AMercySystemTextActor();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|System Text")
	void ShowInstantMessage(const FString& NewMessage, float AutoHideAfterSeconds = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|System Text")
	void ShowTypewriterMessage(const FString& NewMessage, float CharacterIntervalSeconds = 0.04f, float AutoHideAfterSeconds = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|System Text")
	void HideMessage();

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|System Text")
	void SetBlinking(bool bEnableBlinking, float BlinkIntervalSeconds = 0.35f);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|System Text")
	void SetMessageColor(FColor NewColor);

	UFUNCTION(BlueprintCallable, Category = "Mercy Maze|System Text")
	void SetMessageWorldSize(float NewWorldSize);

private:
	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mercy Maze|Components")
	UTextRenderComponent* TextComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|System Text")
	FString InitialMessage = TEXT("DO NOT FOLLOW VOICES");

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|System Text")
	bool bStartHidden = true;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|System Text")
	FColor InitialTextColor = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|System Text", meta = (ClampMin = "1.0"))
	float InitialWorldSize = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Mercy Maze|Debug")
	bool bShowDebugMessages = false;

	FTimerHandle TypewriterTimerHandle;
	FTimerHandle AutoHideTimerHandle;
	FTimerHandle BlinkTimerHandle;

	FString TypewriterFullMessage;
	int32 TypewriterCharacterIndex = 0;

	bool bTextVisible = true;
	bool bBlinkingActive = false;

	void TypewriterTick();
	void BlinkTick();

	void ApplyTextVisibility(bool bShouldBeVisible);
	void ClearAllTextTimers();

	void DebugMessage(const FString& Message, const FColor& Color, float Duration) const;
};