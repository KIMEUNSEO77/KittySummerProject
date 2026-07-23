// Fill out your copyright notice in the Description page of Project Settings.


#include "KittyGameMode.h"

AKittyGameMode::AKittyGameMode()
{
	// DefaultPawnClass
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/KittyProject.KittyCharacterPlayer"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}
	
	// Player Controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerConstrollerClassRef(TEXT("/Script/KittyProject.KittyPlayerController"));
	if (PlayerConstrollerClassRef.Class)
	{
		PlayerControllerClass = PlayerConstrollerClassRef.Class;
	}
}

