#include "MercySystemTextActor.h"

#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

AMercySystemTextActor::AMercySystemTextActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SystemText"));
	TextComponent->SetupAttachment(SceneRoot);

	TextComponent->SetHorizontalAlignment(EHTA_Center);
	TextComponent->SetVerticalAlignment(EVRTA_TextCenter);
	TextComponent->SetTextRenderColor(InitialTextColor);
	TextComponent->SetWorldSize(InitialWorldSize);
	TextComponent->SetText(FText::FromString(InitialMessage));
}

void AMercySystemTextActor::BeginPlay()
{
	Super::BeginPlay();

	if (TextComponent)
	{
		TextComponent->SetText(FText::FromString(InitialMessage));
		TextComponent->SetTextRenderColor(InitialTextColor);
		TextComponent->SetWorldSize(InitialWorldSize);
	}

	ApplyTextVisibility(!bStartHidden);

	DebugMessage(TEXT("MercySystemTextActor ready"), FColor::Cyan, 3.0f);
}

void AMercySystemTextActor::ShowInstantMessage(const FString& NewMessage, float AutoHideAfterSeconds)
{
	ClearAllTextTimers();

	if (TextComponent)
	{
		TextComponent->SetText(FText::FromString(NewMessage));
	}

	ApplyTextVisibility(true);

	if (AutoHideAfterSeconds > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			AutoHideTimerHandle,
			this,
			&AMercySystemTextActor::HideMessage,
			AutoHideAfterSeconds,
			false
		);
	}

	DebugMessage(FString::Printf(TEXT("System text: %s"), *NewMessage), FColor::Yellow, 3.0f);
}

void AMercySystemTextActor::ShowTypewriterMessage(const FString& NewMessage, float CharacterIntervalSeconds, float AutoHideAfterSeconds)
{
	ClearAllTextTimers();

	TypewriterFullMessage = NewMessage;
	TypewriterCharacterIndex = 0;

	if (TextComponent)
	{
		TextComponent->SetText(FText::GetEmpty());
	}

	ApplyTextVisibility(true);

	const float SafeInterval = FMath::Max(0.01f, CharacterIntervalSeconds);

	GetWorldTimerManager().SetTimer(
		TypewriterTimerHandle,
		this,
		&AMercySystemTextActor::TypewriterTick,
		SafeInterval,
		true
	);

	if (AutoHideAfterSeconds > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			AutoHideTimerHandle,
			this,
			&AMercySystemTextActor::HideMessage,
			AutoHideAfterSeconds,
			false
		);
	}

	DebugMessage(FString::Printf(TEXT("Typewriter text: %s"), *NewMessage), FColor::Yellow, 3.0f);
}

void AMercySystemTextActor::HideMessage()
{
	ApplyTextVisibility(false);
	GetWorldTimerManager().ClearTimer(TypewriterTimerHandle);
	GetWorldTimerManager().ClearTimer(AutoHideTimerHandle);
}

void AMercySystemTextActor::SetBlinking(bool bEnableBlinking, float BlinkIntervalSeconds)
{
	bBlinkingActive = bEnableBlinking;

	GetWorldTimerManager().ClearTimer(BlinkTimerHandle);

	if (!bBlinkingActive)
	{
		ApplyTextVisibility(true);
		return;
	}

	const float SafeInterval = FMath::Max(0.05f, BlinkIntervalSeconds);

	GetWorldTimerManager().SetTimer(
		BlinkTimerHandle,
		this,
		&AMercySystemTextActor::BlinkTick,
		SafeInterval,
		true
	);
}

void AMercySystemTextActor::SetMessageColor(FColor NewColor)
{
	if (TextComponent)
	{
		TextComponent->SetTextRenderColor(NewColor);
	}
}

void AMercySystemTextActor::SetMessageWorldSize(float NewWorldSize)
{
	if (TextComponent)
	{
		TextComponent->SetWorldSize(FMath::Max(1.0f, NewWorldSize));
	}
}

void AMercySystemTextActor::TypewriterTick()
{
	if (!TextComponent)
	{
		GetWorldTimerManager().ClearTimer(TypewriterTimerHandle);
		return;
	}

	if (TypewriterCharacterIndex >= TypewriterFullMessage.Len())
	{
		GetWorldTimerManager().ClearTimer(TypewriterTimerHandle);
		return;
	}

	++TypewriterCharacterIndex;

	const FString VisibleMessage = TypewriterFullMessage.Left(TypewriterCharacterIndex);
	TextComponent->SetText(FText::FromString(VisibleMessage));
}

void AMercySystemTextActor::BlinkTick()
{
	ApplyTextVisibility(!bTextVisible);
}

void AMercySystemTextActor::ApplyTextVisibility(bool bShouldBeVisible)
{
	bTextVisible = bShouldBeVisible;

	if (TextComponent)
	{
		TextComponent->SetVisibility(bShouldBeVisible, true);
		TextComponent->SetHiddenInGame(!bShouldBeVisible);
	}
}

void AMercySystemTextActor::ClearAllTextTimers()
{
	GetWorldTimerManager().ClearTimer(TypewriterTimerHandle);
	GetWorldTimerManager().ClearTimer(AutoHideTimerHandle);
}

void AMercySystemTextActor::DebugMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (bShowDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}