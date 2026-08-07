// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KittyCharacterNonplayer.h"
#include "AI/KittyAIController.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AKittyCharacterNonplayer::AKittyCharacterNonplayer()
{
	GetMesh()->SetHiddenInGame(false);
	
	AIControllerClass = AKittyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
}

void AKittyCharacterNonplayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	OnTakeAnyDamage.AddDynamic(
		this,
		&AKittyCharacterNonplayer::OnDamaged);
}

void AKittyCharacterNonplayer::OnDamaged(AActor* DamageActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDead || Damage <= 0.0f)
	{
		return;
	}

	DeathDirection = CalculateDeathDirection(InstigatedBy, DamageCauser);

	Die(GetDeathMontage(DeathDirection));
}

float AKittyCharacterNonplayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AKittyCharacterNonplayer::GetAIDetectRange()
{
	return 400.0f;
}

float AKittyCharacterNonplayer::GetAIAttackRange()
{
	return 0.0f;
}

float AKittyCharacterNonplayer::GetAITurnSpeed()
{
	return 500.0f;
}


EKittyDeathDirection AKittyCharacterNonplayer::CalculateDeathDirection(AController* InstigatedBy,
	AActor* DamageCauser) const
{
	const AActor* Attacker = nullptr;

	if (InstigatedBy)
	{
		Attacker = InstigatedBy->GetPawn();
	}

	if (!IsValid(Attacker))
	{
		Attacker = DamageCauser;
	}

	if (!IsValid(Attacker))
	{
		return EKittyDeathDirection::Front;
	}

	const FVector WorldDirection =
		Attacker->GetActorLocation() - GetActorLocation();

	const FVector LocalDirection =GetActorTransform().InverseTransformVectorNoScale(WorldDirection).GetSafeNormal2D();

	if (FMath::Abs(LocalDirection.X) >= FMath::Abs(LocalDirection.Y))
	{
		return LocalDirection.X >= 0.0f ? EKittyDeathDirection::Front : EKittyDeathDirection::Back;
	}

	return LocalDirection.Y >= 0.0f ? EKittyDeathDirection::Right : EKittyDeathDirection::Left;
}

UAnimMontage* AKittyCharacterNonplayer::GetDeathMontage(EKittyDeathDirection Direction) const
{
	switch (Direction)
	{
	case EKittyDeathDirection::Front:
		return FrontDeathMontage;

	case EKittyDeathDirection::Back:
		return BackDeathMontage;

	case EKittyDeathDirection::Left:
		return LeftDeathMontage;

	case EKittyDeathDirection::Right:
		return RightDeathMontage;

	default:
		return nullptr;
	}
}

void AKittyCharacterNonplayer::Die(UAnimMontage* DeathMontage)
{
	if (bIsDead)
	{
		return;
	}
	
	bIsDead = true;
	SetCanBeDamaged(false);
	
	if (AKittyAIController* AIController = Cast<AKittyAIController>(GetController()))
	{
		AIController->StopMovement();
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		AIController->StopAI();
	}
	
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
}
