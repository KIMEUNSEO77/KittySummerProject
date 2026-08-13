// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/KittyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/KittyCharacterPlayer.h"

UKittyAnimInstance::UKittyAnimInstance()
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;
}

void UKittyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	// Init Variables 
	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UKittyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// Retrieve values from the movement component every frame
	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsCrouching = Movement->IsCrouching();
		if (AKittyCharacterPlayer* Player = Cast<AKittyCharacterPlayer>(Owner))
		{
			bIsJumping = Player->bIsJumping;
			bIsAiming = Player->IsAiming();
		}
	}
}
