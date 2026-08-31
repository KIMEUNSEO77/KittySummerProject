// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KittyCharacterNonplayer.h"
#include "AI/KittyAIController.h"
#include "AI/Attack/KTGunAttackComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Item/KTItemPickupBase.h"
#include "Engine/World.h"

AKittyCharacterNonplayer::AKittyCharacterNonplayer()
{
	GetMesh()->SetHiddenInGame(false);
	
	AIControllerClass = AKittyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	AssassinationAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("AssassinationAnchor"));
	AssassinationAnchor->SetupAttachment(GetRootComponent());
	AssassinationAnchor->SetRelativeLocation(FVector(-90.0f, 0.0f, 0.0f));
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage>FrontDeathMontageRef(TEXT("/Game/Animations/GuardAnimation/AM_FrontDeath1.AM_FrontDeath1"));

	if (FrontDeathMontageRef.Succeeded())
	{
		FrontDeathMontage = FrontDeathMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>BackDeathMontageRef(TEXT("/Game/Animations/GuardAnimation/AM_BackDeath1.AM_BackDeath1"));

	if (BackDeathMontageRef.Succeeded())
	{
		BackDeathMontage = BackDeathMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>LeftDeathMontageRef(TEXT("/Game/Animations/GuardAnimation/AM_LeftDeath1.AM_LeftDeath1"));

	if (LeftDeathMontageRef.Succeeded())
	{
		LeftDeathMontage = LeftDeathMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>RightDeathMontageRef(TEXT("/Game/Animations/GuardAnimation/AM_RightDeath1.AM_RightDeath1"));

	if (RightDeathMontageRef.Succeeded())
	{
		RightDeathMontage = RightDeathMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AssassinatedMontageRef(
		TEXT("/Game/Animations/Assassination/Assassinated.Assassinated")
	);

	if (AssassinatedMontageRef.Succeeded())
	{
		AssassinatedMontage = AssassinatedMontageRef.Object;
	}
	
	BatonAttackComponent = CreateDefaultSubobject<UKTBatonAttackComponent>(TEXT("BatonAttackComponent"));
	
	GunAttackComponent = CreateDefaultSubobject<UKTGunAttackComponent>(TEXT("GunAttackComponent"));
}

void AKittyCharacterNonplayer::BeginPlay()
{
	Super::BeginPlay();
	
	SetGuardWeaponType(GuardWeaponType);
	
	if (!CarriedItemClass || !GetWorld())
	{
		return;
	}

	const FTransform SocketTransform = GetMesh()->GetSocketTransform(CarriedItemSocketName, ERelativeTransformSpace::RTS_World);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CarriedItem = GetWorld()->SpawnActor<AKTItemPickupBase>(CarriedItemClass, SocketTransform, SpawnParams);

	if (!IsValid(CarriedItem))
	{
		return;
	}

	// 살아 있을 때는 획득 불가능
	CarriedItem->SetPickupInteractionEnabled(false);

	// 경비원 허리 소켓에 부착
	CarriedItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CarriedItemSocketName);
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

UKTGuardAttackComponent* AKittyCharacterNonplayer::GetAttackComponent() const
{
	switch (GuardWeaponType)
	{
	case EGuardWeaponType::Baton:
		return BatonAttackComponent;
		
	case EGuardWeaponType::Gun:
		return GunAttackComponent;
		
	default:
		return nullptr;
	}
}

float AKittyCharacterNonplayer::GetAIAttackRange()
{
	const UKTGuardAttackComponent* AttackComponent = GetAttackComponent();
	return AttackComponent ? AttackComponent->GetAttackRange() : 0.0f;
}

void AKittyCharacterNonplayer::SetGuardWeaponType(EGuardWeaponType NewType)
{
	if (GuardWeaponType != NewType)
	{
		StopAnimMontage();
	}
	
	GuardWeaponType = NewType;
	
	BatonAttackComponent->SetActive(NewType == EGuardWeaponType::Baton);
	
	GunAttackComponent->SetActive(NewType == EGuardWeaponType::Gun);
	
	OnGuardWeaponTypeChanged(NewType);
}

bool AKittyCharacterNonplayer::BeginAssassination()
{
	if (bIsDead ||
	bIsBeingAssassinated ||
	!IsValid(AssassinatedMontage))
	{
		return false;
	}
	
	bIsBeingAssassinated = true;
	
	if (AKittyAIController* AIController = Cast<AKittyAIController>(GetController()))
	{
		AIController->StopMovement();
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		AIController->StopAI();
	}
	
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	PreviousMovementMode = static_cast<uint8>(MovementComponent->MovementMode);
	PreviousCustomMovementMode = MovementComponent->CustomMovementMode;
	MovementComponent->StopMovementImmediately();
	MovementComponent->DisableMovement();
	
	PreviousPawnCollisionResponse = GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_Pawn);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	const float Duration = PlayAnimMontage(AssassinatedMontage);
	
	if (Duration <= 0.0f)
	{
		CancelAssassination();
		return false;
	}
	
	return true;
}

void AKittyCharacterNonplayer::CompleteAssassination()
{
	if (!bIsBeingAssassinated||bIsDead)
	{
		return;
	}
	
	bIsBeingAssassinated = false;
	
	Die(nullptr);
}

void AKittyCharacterNonplayer::CancelAssassination()
{
	if (!bIsBeingAssassinated || bIsDead)
	{
		return;
	}

	bIsBeingAssassinated = false;
	StopAnimMontage(AssassinatedMontage);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn,PreviousPawnCollisionResponse);

	GetCharacterMovement()->SetMovementMode(
		static_cast<EMovementMode>(PreviousMovementMode),
		PreviousCustomMovementMode
	);

	if (AKittyAIController* AIController = Cast<AKittyAIController>(GetController()))
	{
		AIController->RunAI();
	}
}

FTransform AKittyCharacterNonplayer::GetAssassinationAnchorTransform() const
{
	return IsValid(AssassinationAnchor)
		? AssassinationAnchor->GetComponentTransform()
		: GetActorTransform();
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
	// 증원 매니저에 사망 사실 전달
	OnGuardDied.Broadcast();
	SetCanBeDamaged(false);
	if (IsValid(CarriedItem))
	{
		CarriedItem->SetPickupInteractionEnabled(true);
	}
	
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

void AKittyCharacterNonplayer::DebugKill()
{
	if (bIsDead)
	{
		return;
	}
	DeathDirection = DebugDirection;
	
	Die(GetDeathMontage(DeathDirection));
}

void AKittyCharacterNonplayer::ConfigurePatrolRoute(const TArray<ATargetPoint*>& InPatrolPoints)
{
	PatrolPoints.Reset();
	
	for (ATargetPoint* Point : InPatrolPoints)
	{
		if (IsValid(Point))
		{
			PatrolPoints.Add(Point);
		}
	}
	
	bPatrolEnabled = !PatrolPoints.IsEmpty();
	
	if (AKittyAIController* AIController = Cast<AKittyAIController>(GetController()))
	{
		AIController->StopAI();
		AIController->RunAI();
	}
}
