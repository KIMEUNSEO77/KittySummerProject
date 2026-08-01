// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/KittyCharacterPlayer.h"

#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "KittyCharacterControlData.h"
#include "UObject/ConstructorHelpers.h"
#include "Interface/KTInteractableInterface.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"

AKittyCharacterPlayer::AKittyCharacterPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Input
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Game/MyInput/Input/Actions/IA_Jump.IA_Jump"));
	if (InputActionJumpRef.Succeeded())
	{
		JumpAction = InputActionJumpRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeControlRef(TEXT("/Game/MyInput/Input/Actions/IA_ChangeControl.IA_ChangeControl"));
	if (InputChangeControlRef.Succeeded())
	{
		ChangeControlAction = InputChangeControlRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Game/MyInput/Input/Actions/IA_ShoulderMove.IA_ShoulderMove"));
	if (InputActionShoulderMoveRef.Succeeded())
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Game/MyInput/Input/Actions/IA_ShoulderLook.IA_ShoulderLook"));
	if (InputActionShoulderLookRef.Succeeded())
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuaterMoveRef(TEXT("/Game/MyInput/Input/Actions/IA_QuaterMove.IA_QuaterMove"));
	if (InputActionQuaterMoveRef.Succeeded())
	{
		QuaterMoveAction = InputActionQuaterMoveRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> MouseLookActionRef(TEXT("/Game/MyInput/Input/Actions/IA_MouseLook.IA_MouseLook"));
	if (MouseLookActionRef.Succeeded())
	{
		MouseLookAction = MouseLookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MouseLookContextRef(TEXT("/Game/MyInput/Input/IMC_MouseLook.IMC_MouseLook"));
	if (MouseLookContextRef.Succeeded())
	{
		MouseLookMappingContext = MouseLookContextRef.Object;
	}
	
	CurrentCharacterControlType = ECharacterControlType::Shoulder;
}

void AKittyCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	SetCharacterControl(CurrentCharacterControlType);
}

void AKittyCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CheckForInteractable();
}

void AKittyCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Started,this,&AKittyCharacterPlayer::JumpStart);
	EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Completed,this,&AKittyCharacterPlayer::JumpEnd);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AKittyCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AKittyCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AKittyCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AKittyCharacterPlayer::QuaterMove);
	
	EnhancedInputComponent->BindAction(MouseLookAction,ETriggerEvent::Triggered,this,&AKittyCharacterPlayer::ShoulderLook);
}

void AKittyCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quater);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Quater)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
}

void AKittyCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UKittyCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];

	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;

		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
		
		if (NewCharacterControlType == ECharacterControlType::Shoulder && MouseLookMappingContext)
		{
			Subsystem->AddMappingContext(MouseLookMappingContext, 1);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AKittyCharacterPlayer::SetCharacterControlData(const class UKittyCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);
	
	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}

void AKittyCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0, Rotation.Yaw, 0.0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AKittyCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AKittyCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();
	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}
	
	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AKittyCharacterPlayer::JumpStart()
{
	bIsJumping = true;
	Jump();
}

void AKittyCharacterPlayer::JumpEnd()
{
	bIsJumping = false;
	StopJumping();
}

void AKittyCharacterPlayer::CheckForInteractable()
{
	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const FVector SearchLocation = GetActorLocation();
	const float SearchRadius = InteractionDistance;

	const bool bFoundActors = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		SearchLocation,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(SearchRadius),
		QueryParams
	);

	AActor* BestInteractable = nullptr;
	float BestDistanceSquared = TNumericLimits<float>::Max();

	if (bFoundActors)
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* Candidate = OverlapResult.GetActor();

			if (!IsValid(Candidate))
			{
				continue;
			}

			// 상호작용 인터페이스가 없는 Actor는 제외
			if (!Candidate->GetClass()->ImplementsInterface(
				UKTInteractableInterface::StaticClass()))
			{
				continue;
			}

			FVector ToCandidate = Candidate->GetActorLocation() - GetActorLocation();

			// 높이 차이는 방향 검사에서 제외
			ToCandidate.Z = 0.0f;

			if (ToCandidate.IsNearlyZero())
			{
				continue;
			}

			const FVector DirectionToCandidate = ToCandidate.GetSafeNormal();

			const FVector PlayerForward = GetActorForwardVector().GetSafeNormal2D();

			const float ForwardDot = FVector::DotProduct(PlayerForward, DirectionToCandidate);

			// 캐릭터 뒤쪽에 있는 아이템은 제외
			if (ForwardDot < 0.2f)
			{
				continue;
			}

			const float DistanceSquared = ToCandidate.SizeSquared();

			// 앞쪽에 있는 아이템 중 가장 가까운 것을 선택
			if (DistanceSquared < BestDistanceSquared)
			{
				BestDistanceSquared = DistanceSquared;
				BestInteractable = Candidate;
			}
		}
	}

	CurrentInteractable = BestInteractable;

	if (IsValid(CurrentInteractable) && GEngine)
	{
		const FText PromptText =
			IKTInteractableInterface::Execute_GetInteractionText(
				CurrentInteractable
			);

		GEngine->AddOnScreenDebugMessage(
			1,
			0.0f,
			FColor::Yellow,
			PromptText.ToString()
		);
	}
}
