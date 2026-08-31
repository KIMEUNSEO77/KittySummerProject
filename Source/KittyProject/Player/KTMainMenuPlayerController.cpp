#include "Player/KTMainMenuPlayerController.h"

#include "Blueprint/UserWidget.h"

void AKTMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	if (!MainMenuWidgetClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("MainMenuWidgetClass가 설정되지 않았습니다.")
		);

		return;
	}

	MainMenuWidget = CreateWidget<UUserWidget>(
		this,
		MainMenuWidgetClass
	);

	if (!MainMenuWidget)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("메인메뉴 위젯 생성에 실패했습니다.")
		);

		return;
	}

	MainMenuWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	bShowMouseCursor = true;
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
}