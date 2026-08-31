#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KTMainMenuPlayerController.generated.h"

class UUserWidget;

UCLASS()
class KITTYPROJECT_API AKTMainMenuPlayerController
	: public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// 에디터의 BP_MainMenuPlayerController에서
	// WBP_MainMenu를 연결
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "UI"
	)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	// 실제 화면에 생성된 메인메뉴 위젯
	UPROPERTY()
	TObjectPtr<UUserWidget> MainMenuWidget;
};