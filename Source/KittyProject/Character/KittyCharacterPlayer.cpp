// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/KittyCharacterPlayer.h"

#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequence.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "KittyCharacterControlData.h"
#include "UObject/ConstructorHelpers.h"
#include "Interface/KTInteractableInterface.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Item/KTPistolPickup.h"
#include "Inventory/KTInventoryComponent.h"
#include "Player/KittyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"
#include "Item/KTItemPickupBase.h"
#include "Animation/AnimMontage.h"
#include "MotionWarpingComponent.h"
#include "Interaction/KTKeycardDoor.h"

AKittyCharacterPlayer::AKittyCharacterPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	InventoryComponent = CreateDefaultSubobject<UKTInventoryComponent>(
		TEXT("InventoryComponent")
	);
	
	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 100.0f;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.0f);

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
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionInteractionRef(TEXT("/Game/MyInput/Input/Actions/IA_Interaction.IA_Interaction"));
	if (InputActionInteractionRef.Succeeded())
	{
		InteractionAction = InputActionInteractionRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAimRef(TEXT("/Game/MyInput/Input/Actions/IA_Aim.IA_Aim"));
	if (InputActionAimRef.Succeeded())
	{
		AimAction = InputActionAimRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFireRef(TEXT("/Game/MyInput/Input/Actions/IA_Fire.IA_Fire"));
	if (InputActionFireRef.Succeeded())
	{
		FireAction = InputActionFireRef.Object;
	}
	
	CurrentCharacterControlType = ECharacterControlType::Shoulder;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionInventoryRef(TEXT("/Game/MyInput/Input/Actions/IA_Inventory.IA_Inventory"));

	if (InputActionInventoryRef.Succeeded())
	{
		InventoryAction = InputActionInventoryRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionCrouchRef(TEXT("/Game/MyInput/Input/Actions/IA_Crouch.IA_Crouch"));

	if (InputActionCrouchRef.Succeeded())
	{
		CrouchAction = InputActionCrouchRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> StandingToCrouchedAnimationRef(TEXT("/Game/Animations/Crouch/Standing_To_Crouched.Standing_To_Crouched"));

	if (StandingToCrouchedAnimationRef.Succeeded())
	{
		StandingToCrouchedAnimation = StandingToCrouchedAnimationRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> CrouchedToStandingAnimationRef(TEXT("/Game/Animations/Crouch/Crouched_To_Standing.Crouched_To_Standing"));

	if (CrouchedToStandingAnimationRef.Succeeded())
	{
		CrouchedToStandingAnimation = CrouchedToStandingAnimationRef.Object;
	}
	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	
	KeycardUseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeycardUseMesh"));
	KeycardUseMesh->SetupAttachment(GetMesh(), TEXT("ItemPickupSocket"));
	KeycardUseMesh->SetVisibility(false);
	KeycardUseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	KeycardUseMesh->SetCastShadow(false);
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
	UpdateInventoryCamera(DeltaTime);
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
	
	EnhancedInputComponent->BindAction(InteractionAction,ETriggerEvent::Started,this,&AKittyCharacterPlayer::Interact);
	
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AKittyCharacterPlayer::StartAiming);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AKittyCharacterPlayer::StopAiming);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AKittyCharacterPlayer::Fire);
	EnhancedInputComponent->BindAction(InventoryAction,ETriggerEvent::Started,this,&AKittyCharacterPlayer::ToggleInventory);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AKittyCharacterPlayer::CrouchStart);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AKittyCharacterPlayer::CrouchEnd);

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

void AKittyCharacterPlayer::CrouchStart()
{
	const bool bWasCrouching = GetCharacterMovement()->IsCrouching();
	Crouch();

	if (!bWasCrouching && StandingToCrouchedAnimation)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->PlaySlotAnimationAsDynamicMontage(StandingToCrouchedAnimation, TEXT("DefaultSlot"));
		}
	}
}

void AKittyCharacterPlayer::CrouchEnd()
{
	const bool bWasCrouching = GetCharacterMovement()->IsCrouching();
	UnCrouch();

	if (bWasCrouching && !GetCharacterMovement()->IsCrouching() && CrouchedToStandingAnimation)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->PlaySlotAnimationAsDynamicMontage(CrouchedToStandingAnimation, TEXT("DefaultSlot"));
		}
	}
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

	const bool bFoundActors = GetWorld()->OverlapMultiByObjectType(OverlapResults, SearchLocation, FQuat::Identity, ObjectQueryParams,
		FCollisionShape::MakeSphere(SearchRadius), QueryParams);

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
			if (!Candidate->GetClass()->ImplementsInterface(UKTInteractableInterface::StaticClass()))
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
			
			if (const AKTItemPickupBase* PickupItem = Cast<AKTItemPickupBase>(Candidate))
			{
				const float AllowedDistance = PickupItem->GetPickupInteractionDistance();

				if (DistanceSquared > FMath::Square(AllowedDistance))
				{
					continue;
				}
			}

			// 앞쪽에 있는 아이템 중 가장 가까운 것을 선택
			if (DistanceSquared < BestDistanceSquared)
			{
				BestDistanceSquared = DistanceSquared;
				BestInteractable = Candidate;
			}
		}
	}

	CurrentInteractable = BestInteractable;

	AKittyPlayerController* KittyPlayerController =
		Cast<AKittyPlayerController>(GetController());

	if (!KittyPlayerController)
	{
		return;
	}

	if (IsValid(CurrentInteractable))
	{
		const FText PromptText =
			IKTInteractableInterface::Execute_GetInteractionText(
				CurrentInteractable
			);

		KittyPlayerController->ShowObjectiveInteractionPrompt(
			CurrentInteractable,
			PromptText
		);
	}
	else
	{
		KittyPlayerController->HideObjectiveInteractionPrompt();
	}
	
}

void AKittyCharacterPlayer::Interact()
{
	// 현재 상호작용 대상이 없으면 아무것도 하지 않음
	if (!IsValid(CurrentInteractable))
	{
		return;
	}

	// 대상이 상호작용 인터페이스를 구현했는지 확인
	if (!CurrentInteractable->GetClass()->ImplementsInterface(UKTInteractableInterface::StaticClass()))
	{
		return;
	}
	
	if (InteractionSound)
	{
		UGameplayStatics::PlaySound2D(
			this,
			InteractionSound
		);
	}
	
	// 인터페이스를 통해 대상의 상호작용 함수 호출
	IKTInteractableInterface::Execute_Interact(CurrentInteractable, this);


}

bool AKittyCharacterPlayer::AcquirePistol(class AActor* PistolActor)
{
	if (!IsValid(PistolActor))
	{
		return false;
	}

	// 이미 권총을 가지고 있다면 획득하지 않음
	if (bHasPistol)
	{
		return false;
	}

	USkeletalMeshComponent* CharacterMesh = GetMesh();

	if (!IsValid(CharacterMesh))
	{
		return false;
	}

	const FName PistolSocketName(TEXT("PistolSocket"));

	// 소켓 이름이 잘못되었을 경우 장착하지 않음
	if (!CharacterMesh->DoesSocketExist(PistolSocketName))
	{
		UE_LOG(LogTemp, Warning, TEXT("PistolSocket이 캐릭터 Mesh에 없습니다."));
		return false;
	}

	// 바닥에서 사용하던 충돌을 모두 비활성화
	PistolActor->SetActorEnableCollision(false);

	// 플레이어가 권총 Actor의 소유자가 됨
	PistolActor->SetOwner(this);

	const bool bAttached = PistolActor->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, PistolSocketName);

	if (!bAttached)
	{
		PistolActor->SetActorEnableCollision(true);
		PistolActor->SetOwner(nullptr);
		return false;
	}

	EquippedPistol = PistolActor;
	bHasPistol = true;

	return true;
}

void AKittyCharacterPlayer::StartAiming()
{
	// 권총을 획득하지 않았다면 조준할 수 없음
	if (!bHasPistol)
	{
		return;
	}

	bIsAiming = true;
}

void AKittyCharacterPlayer::StopAiming()
{
	bIsAiming = false;
	bIsFiring = false;

	GetWorldTimerManager().ClearTimer(FireAnimationTimerHandle);
}

void AKittyCharacterPlayer::Fire()
{
	if (!bHasPistol || !bIsAiming || bIsFiring)
	{
		return;
	}

	// 발사 쿨타임 시작
	bIsFiring = true;

	// 실제 발사 판정
	PerformFireTrace();

	GetWorldTimerManager().SetTimer(FireAnimationTimerHandle, this, &AKittyCharacterPlayer::StopFiring, FireAnimationDuration, false);
}

void AKittyCharacterPlayer::StopFiring()
{
	bIsFiring = false;
}

void AKittyCharacterPlayer::PerformFireTrace()
{
	UWorld* World = GetWorld();

	if (!IsValid(World)) return;

	AKTPistolPickup* Pistol = Cast<AKTPistolPickup>(EquippedPistol);

	if (!IsValid(Pistol)) return;
	
	Pistol->PlayMuzzleFlash();   // 이펙트 재생
	Pistol->PlayFireSound();     // 사운드 재생
	
	if (FireCameraShakeClass)    // 카메라 흔들림
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerController->ClientStartCameraShake(FireCameraShakeClass, FireCameraShakeScale);
		}
	}

	const FVector TraceStart = Pistol->GetMuzzleLocation();
	const FVector TraceDirection = Pistol->GetMuzzleForwardVector();
	const FVector TraceEnd = TraceStart + TraceDirection * FireRange;

	FHitResult HitResult;
	
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PistolFireTrace), true, this);

	// 장착된 권총도 무시
	QueryParams.AddIgnoredActor(Pistol);

	const bool bHit = World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	const FVector DebugEnd = bHit ? HitResult.ImpactPoint : TraceEnd;

	//DrawDebugLine(World, TraceStart, DebugEnd, bHit ? FColor::Green : FColor::Red, false, 1.0f, 0, 1.5f);

	if (bHit)
	{
		//DrawDebugSphere(World,HitResult.ImpactPoint,8.0f,12,FColor::Yellow,false,1.0f);
	}

	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;
	
	if (IsValid(HitActor))
	{
		UGameplayStatics::ApplyPointDamage(
			HitActor,                  // 총에 맞은 Actor
			1.0f,                      // 데미지
			TraceDirection,            // 총알 진행 방향
			HitResult,                 // 충돌 결과
			GetController(),           // 공격한 플레이어의 Controller
			Pistol,                    // 데미지를 발생시킨 권총
			UDamageType::StaticClass()
		);
	}

	//if (IsValid(HitActor) && GEngine)
	//{
		//GEngine->AddOnScreenDebugMessage(20, 1.5f, FColor::Cyan, FString::Printf(TEXT("명중: %s"), *HitActor->GetName()));
	//}
}

void AKittyCharacterPlayer::ToggleInventory()
{
	AKittyPlayerController* KittyPlayerController = Cast<AKittyPlayerController>(GetController());

	if (KittyPlayerController)
	{
		KittyPlayerController->ToggleInventory();
	}
}

void AKittyCharacterPlayer::EnterInventoryCamera()
{
	if (!CameraBoom || !FollowCamera)
	{
		return;
	}

	// 인벤토리를 처음 열 때만 현재 게임 카메라를 저장합니다.
	if (!bIsInventoryCameraActive)
	{
		NormalCameraArmLength = CameraBoom->TargetArmLength;
		NormalCameraSocketOffset = CameraBoom->SocketOffset;
		NormalCameraFOV = FollowCamera->FieldOfView;
	}

	CameraTransitionStartArmLength = CameraBoom->TargetArmLength;
	CameraTransitionStartSocketOffset = CameraBoom->SocketOffset;
	CameraTransitionStartFOV = FollowCamera->FieldOfView;

	InventoryCameraTransitionElapsed = 0.0f;
	bIsInventoryCameraActive = true;
	bIsInventoryCameraTransitioning = true;
}

void AKittyCharacterPlayer::ExitInventoryCamera()
{
	if (!CameraBoom || !FollowCamera)
	{
		return;
	}

	CameraTransitionStartArmLength = CameraBoom->TargetArmLength;
	CameraTransitionStartSocketOffset = CameraBoom->SocketOffset;
	CameraTransitionStartFOV = FollowCamera->FieldOfView;

	InventoryCameraTransitionElapsed = 0.0f;
	bIsInventoryCameraActive = false;
	bIsInventoryCameraTransitioning = true;
}

void AKittyCharacterPlayer::UpdateInventoryCamera(float DeltaTime)
{
	if (!CameraBoom || !FollowCamera)
	{
		return;
	}
	if (!bIsInventoryCameraTransitioning)
	{
		return;
	}
	// 열기/닫기 전환이 끝난 상태면 더 계산하지 않습니다.
	const bool bAlreadyAtInventoryCamera =
		bIsInventoryCameraActive &&
		FMath::IsNearlyEqual(
			CameraBoom->TargetArmLength,
			InventoryCameraArmLength,
			0.1f
		);

	const bool bAlreadyAtNormalCamera =
		!bIsInventoryCameraActive &&
		FMath::IsNearlyEqual(
			CameraBoom->TargetArmLength,
			NormalCameraArmLength,
			0.1f
		);

	if (bAlreadyAtInventoryCamera || bAlreadyAtNormalCamera)
	{
		return;
	}

	InventoryCameraTransitionElapsed += DeltaTime;

	const float Alpha = FMath::Clamp(
		InventoryCameraTransitionElapsed /
		InventoryCameraTransitionDuration,
		0.0f,
		1.0f
	);

	// 시작과 끝이 부드러운 0.2초 카메라 이동입니다.
	const float SmoothAlpha = FMath::InterpEaseInOut(
		0.0f,
		1.0f,
		Alpha,
		2.0f
	);

	const float TargetArmLength =
		bIsInventoryCameraActive
			? InventoryCameraArmLength
			: NormalCameraArmLength;

	const FVector TargetSocketOffset =
		bIsInventoryCameraActive
			? InventoryCameraSocketOffset
			: NormalCameraSocketOffset;

	const float TargetFOV =
		bIsInventoryCameraActive
			? InventoryCameraFOV
			: NormalCameraFOV;

	CameraBoom->TargetArmLength = FMath::Lerp(
		CameraTransitionStartArmLength,
		TargetArmLength,
		SmoothAlpha
	);

	CameraBoom->SocketOffset = FMath::Lerp(
		CameraTransitionStartSocketOffset,
		TargetSocketOffset,
		SmoothAlpha
	);

	FollowCamera->SetFieldOfView(
		FMath::Lerp(
			CameraTransitionStartFOV,
			TargetFOV,
			SmoothAlpha
		)
	);
	
	if (Alpha >= 1.0f)
	{
		bIsInventoryCameraTransitioning = false;
	}
}

void AKittyCharacterPlayer::StartPistolPickup(AKTPistolPickup* PistolPickup)
{
	if (bIsPerformingInteraction || !IsValid(PistolPickup) || !IsValid(PistolPickupMontage) || !IsValid(MotionWarpingComponent))
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(AnimInstance))
	{
		return;
	}

	PendingPistolPickup = PistolPickup;

	// 조준 상태 해제
	StopAiming();
	
	const FTransform PickupTargetTransform = PistolPickup->GetPickupStandTransform();

	MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform(TEXT("PistolPickupTarget"), PickupTargetTransform);

	// 이동과 카메라 조작 잠금
	BeginInteractionLock();
	
	AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AKittyCharacterPlayer::HandleInteractionNotify);
	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AKittyCharacterPlayer::HandleInteractionMontageEnded);

	const float PlayedDuration = AnimInstance->Montage_Play(PistolPickupMontage);

	// 몽타주 재생에 실패했다면 잠금 해제
	if (PlayedDuration <= 0.0f)
	{
		PendingPistolPickup = nullptr;
		MotionWarpingComponent->RemoveWarpTarget(TEXT("PistolPickupTarget"));
		EndInteractionLock();
	}
}

void AKittyCharacterPlayer::StartItemPickup(class AKTItemPickupBase* ItemPickup, class UAnimMontage* ItemMontage)
{
	if (bIsPerformingInteraction || !IsValid(ItemPickup) || !IsValid(ItemMontage))
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(AnimInstance))
	{
		return;
	}

	PendingItemPickup = ItemPickup;
	ActiveItemPickupMontage = ItemMontage;

	StopAiming();

	// 제자리 애니메이션이므로 아이템을 바라보게만 함
	FVector DirectionToItem = ItemPickup->GetActorLocation() - GetActorLocation();

	DirectionToItem.Z = 0.0f;

	if (!DirectionToItem.IsNearlyZero())
	{
		const FRotator TargetRotation = DirectionToItem.Rotation();

		SetActorRotation(FRotator(0.0f, TargetRotation.Yaw, 0.0f));
	}

	BeginInteractionLock();

	AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AKittyCharacterPlayer::HandleInteractionNotify);
	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AKittyCharacterPlayer::HandleInteractionMontageEnded);

	const float PlayedDuration = AnimInstance->Montage_Play(ItemMontage);

	if (PlayedDuration <= 0.0f)
	{
		PendingItemPickup = nullptr;
		ActiveItemPickupMontage = nullptr;
		EndInteractionLock();
	}
}

void AKittyCharacterPlayer::StartKeycardDoorInteraction(class AKTKeycardDoor* KeycardDoor)
{
	if (bIsPerformingInteraction || !IsValid(KeycardDoor) || !IsValid(KeycardUseMontage))
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(AnimInstance))
	{
		return;
	}

	PendingKeycardDoor = KeycardDoor;

	// 조준 중이었다면 조준 해제
	StopAiming();

	// 이동과 카메라 조작 잠금
	BeginInteractionLock();

	AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AKittyCharacterPlayer::HandleInteractionNotify);

	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AKittyCharacterPlayer::HandleInteractionMontageEnded);

	const float PlayedDuration = AnimInstance->Montage_Play(KeycardUseMontage);

	// 몽타주 재생 실패 시 정리
	if (PlayedDuration <= 0.0f)
	{
		PendingKeycardDoor = nullptr;
		EndInteractionLock();
	}
}

void AKittyCharacterPlayer::BeginInteractionLock()
{
	bIsPerformingInteraction = true;

	GetCharacterMovement()->StopMovementImmediately();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);
	}
}

void AKittyCharacterPlayer::EndInteractionLock()
{
	bIsPerformingInteraction = false;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
	}
}

void AKittyCharacterPlayer::HandleInteractionNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (!bIsPerformingInteraction)
	{
		return;
	}
	
	// 손에 출입증 표시
	if (NotifyName == TEXT("ShowKeycard"))
	{
		if (IsValid(KeycardUseMesh))
		{
			KeycardUseMesh->SetVisibility(true);
		}

		return;
	}
	
	// 출입증을 단말기에 대는 순간
	if (NotifyName == TEXT("UseKeycard"))
	{
		if (IsValid(PendingKeycardDoor))
		{
			PendingKeycardDoor->CompleteKeycardInteraction();
		}

		return;
	}
	
	// 손에 표시한 출입증 숨기기
	if (NotifyName == TEXT("HideKeycard"))
	{
		if (IsValid(KeycardUseMesh))
		{
			KeycardUseMesh->SetVisibility(false);
		}

		return;
	}

	// 권총 획득 Notify
	if (NotifyName == TEXT("PickupPistol"))
	{
		if (IsValid(PendingPistolPickup))
		{
			if (PendingPistolPickup->CompletePickup(this))
			{
				PendingPistolPickup = nullptr;
			}
		}

		return;
	}

	// 일반 아이템 휙득 Notify
	if (NotifyName == TEXT("PickupItem"))
	{
		if (IsValid(PendingItemPickup))
		{
			// 다시 상호작용되지 않도록 충돌 비활성화
			PendingItemPickup->SetPickupInteractionEnabled(false);

			// 카드키를 왼손 소켓에 임시 부착
			PendingItemPickup->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("ItemPickupSocket"));
		}

		return;
	}
	
	// 아이템 저장
	if (NotifyName == TEXT("StoreItem"))
	{
		if (IsValid(PendingItemPickup))
		{
			if (PendingItemPickup->CompletePickup(this))
			{
				PendingItemPickup = nullptr;

				if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
				{
					AnimInstance->Montage_Stop(0.5f, ActiveItemPickupMontage);
				}
			}
		}

		return;
	}
}

void AKittyCharacterPlayer::HandleInteractionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const bool bPistolMontageEnded = Montage == PistolPickupMontage;
	const bool bItemMontageEnded = Montage == ActiveItemPickupMontage;
	const bool bKeycardMontageEnded = Montage == KeycardUseMontage;

	if (!bPistolMontageEnded && !bItemMontageEnded && !bKeycardMontageEnded)
	{
		return;
	}

	if (bPistolMontageEnded && IsValid(MotionWarpingComponent))
	{
		MotionWarpingComponent->RemoveWarpTarget(TEXT("PistolPickupTarget"));
	}
	
	if (bKeycardMontageEnded && IsValid(KeycardUseMesh))
	{
		KeycardUseMesh->SetVisibility(false);
	}

	PendingPistolPickup = nullptr;
	PendingItemPickup = nullptr;
	ActiveItemPickupMontage = nullptr;
	PendingKeycardDoor = nullptr;

	EndInteractionLock();
}
