// Fill out your copyright notice in the Description page of Project Settings.


#include "KittyGameMode.h"

AKittyGameMode::AKittyGameMode()
{
	// DefaultPawnClass
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/Blueprint/BP_KittyCharacterPlayer"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}
	
	// Player Controller
	// 경로 교체 (UI 작업 .0804)
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerConstrollerClassRef(TEXT("/Game/Blueprint/BP_KittyPlayerController.BP_KittyPlayerController_C"));
	if (PlayerConstrollerClassRef.Class)
	{
		PlayerControllerClass = PlayerConstrollerClassRef.Class;
	}
}

