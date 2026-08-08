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
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Item/KTPistolPickup.h"
#include "Inventory/KTInventoryComponent.h"
#include "Player/KittyPlayerController.h"

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
	
	EnhancedInputComponent->BindAction(InteractionAction,ETriggerEvent::Started,this,&AKittyCharacterPlayer::Interact);
	
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AKittyCharacterPlayer::StartAiming);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AKittyCharacterPlayer::StopAiming);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AKittyCharacterPlayer::Fire);
	EnhancedInputComponent->BindAction(InventoryAction,ETriggerEvent::Started,this,&AKittyCharacterPlayer::ToggleInventory);

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

	// 인터페이스를 통해 대상의 상호작용 함수 호출
	IKTInteractableInterface::Execute_Interact(CurrentInteractable, this);

	// 동작 확인을 위한 임시 메시지
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Green, TEXT("상호작용 실행 성공"));
	}
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
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PistolSocket이 캐릭터 Mesh에 없습니다.")
		);

		return false;
	}

	// 바닥에서 사용하던 충돌을 모두 비활성화
	PistolActor->SetActorEnableCollision(false);

	// 플레이어가 권총 Actor의 소유자가 됨
	PistolActor->SetOwner(this);

	const bool bAttached = PistolActor->AttachToComponent(
		CharacterMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		PistolSocketName
	);

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

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			10,
			2.0f,
			FColor::Green,
			TEXT("조준 시작")
		);
	}
}

void AKittyCharacterPlayer::StopAiming()
{
	bIsAiming = false;
	bIsFiring = false;

	GetWorldTimerManager().ClearTimer(
		FireAnimationTimerHandle
	);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			10,
			2.0f,
			FColor::White,
			TEXT("조준 종료")
		);
	}
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

	GetWorldTimerManager().SetTimer(
		FireAnimationTimerHandle,
		this,
		&AKittyCharacterPlayer::StopFiring,
		FireAnimationDuration,
		false
	);
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

	const FVector TraceStart = Pistol->GetMuzzleLocation();
	const FVector TraceDirection = Pistol->GetMuzzleForwardVector();
	const FVector TraceEnd = TraceStart + TraceDirection * FireRange;

	FHitResult HitResult;

	// 세 번째 인자인 this로 플레이어 자신을 무시
	FCollisionQueryParams QueryParams(
		SCENE_QUERY_STAT(PistolFireTrace),
		true,
		this
	);

	// 장착된 권총도 무시
	QueryParams.AddIgnoredActor(Pistol);

	const bool bHit =
		World->LineTraceSingleByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			QueryParams
		);

	const FVector DebugEnd = bHit ? HitResult.ImpactPoint : TraceEnd;

	DrawDebugLine(
		World,
		TraceStart,
		DebugEnd,
		bHit ? FColor::Green : FColor::Red,
		false,
		1.0f,
		0,
		1.5f
	);

	if (bHit)
	{
		DrawDebugSphere(
			World,
			HitResult.ImpactPoint,
			8.0f,
			12,
			FColor::Yellow,
			false,
			1.0f
		);
	}

	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;

	if (IsValid(HitActor) && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			20,
			1.5f,
			FColor::Cyan,
			FString::Printf(
				TEXT("명중: %s"),
				*HitActor->GetName()
			)
		);
	}
}

void AKittyCharacterPlayer::ToggleInventory()
{
	AKittyPlayerController* KittyPlayerController =
		Cast<AKittyPlayerController>(GetController());

	if (KittyPlayerController)
	{
		KittyPlayerController->ToggleInventory();
	}
}
