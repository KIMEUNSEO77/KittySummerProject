// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTSafe.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

#include "UI/KTSafeKeypadWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AKTSafe::AKTSafe()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// 기본 루트
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// 금고 본체
	SafeBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SafeBodyMesh"));
	SafeBodyMesh->SetupAttachment(SceneRoot);

	// 문의 경첩 역할을 할 회전축
	DoorPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorPivot->SetupAttachment(SceneRoot);

	// 금고 문
	SafeDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SafeDoorMesh"));
	SafeDoorMesh->SetupAttachment(DoorPivot);

	// 상호작용 감지용 충돌체
	InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(SceneRoot);
	InteractionCollision->InitSphereRadius(100.0f);

	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
}

void AKTSafe::Interact_Implementation(AActor* Interactor)
{
	if (!IsValid(Interactor) || bIsOpen)
	{
		return;
	}

	// 상호작용한 플레이어 캐릭터를 Pawn으로 변환
	APawn* PlayerPawn = Cast<APawn>(Interactor);

	if (!IsValid(PlayerPawn))
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn->GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}
	
	if (!SafeKeypadWidgetClass)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				3.0f,
				FColor::Red,
				TEXT("SafeKeypadWidgetClass가 설정되지 않았습니다.")
			);
		}

		return;
	}

	// 아직 위젯을 만들지 않았다면 새로 생성
	if (!IsValid(SafeKeypadWidget))
	{
		SafeKeypadWidget = CreateWidget<UKTSafeKeypadWidget>(PlayerController, SafeKeypadWidgetClass);
	}

	if (!IsValid(SafeKeypadWidget))
	{
		return;
	}
	
	// 이 키패드가 어느 금고의 비밀번호를 검사할지 알려줌
	SafeKeypadWidget->SetOwningSafe(this);
	SafeKeypadWidget->ResetPassword();

	// 위젯이 아직 화면에 없다면 추가
	if (!SafeKeypadWidget->IsInViewport())
	{
		SafeKeypadWidget->AddToViewport(50);
	}

	SafeKeypadWidget->SetVisibility(ESlateVisibility::Visible);

	// 마우스로 UI 버튼을 누를 수 있도록 입력 모드를 변경
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(SafeKeypadWidget->TakeWidget());

	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
}

FText AKTSafe::GetInteractionText_Implementation() const
{
	if (bIsOpen)
	{
		return FText::FromString(TEXT("금고가 열려 있습니다"));
	}

	return FText::FromString(TEXT("[F] 금고 비밀번호 입력하기"));
}

void AKTSafe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsOpening || !IsValid(DoorPivot))
	{
		return;
	}

	FRotator TargetRotation = ClosedDoorRotation;
	TargetRotation.Yaw += OpenAngle;

	const FRotator CurrentRotation =
		DoorPivot->GetRelativeRotation();

	const FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		TargetRotation,
		DeltaTime,
		DoorOpenSpeed
	);

	DoorPivot->SetRelativeRotation(NewRotation);

	// 목표 각도에 거의 도착하면 정확한 각도로 맞추고 Tick을 중단
	if (NewRotation.Equals(TargetRotation, 0.1f))
	{
		DoorPivot->SetRelativeRotation(TargetRotation);

		bIsOpening = false;
		SetActorTickEnabled(false);
	}
}

bool AKTSafe::TryUnlock(const FString& EnteredPassword)
{
	if (bIsOpen)
	{
		return true;
	}

	if (EnteredPassword != CorrectPassword)
	{
		return false;
	}

	bIsOpen = true;
	bIsOpening = true;

	// 문 애니메이션이 실행되도록 Tick을 실행
	SetActorTickEnabled(true);

	CloseKeypad();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Green,
			TEXT("비밀번호가 일치합니다. 금고가 열립니다.")
		);
	}

	return true;
}

void AKTSafe::CloseKeypad()
{
	if (!IsValid(SafeKeypadWidget))
	{
		return;
	}

	APlayerController* PlayerController = SafeKeypadWidget->GetOwningPlayer();

	SafeKeypadWidget->SetVisibility(ESlateVisibility::Collapsed);

	if (!IsValid(PlayerController))
	{
		return;
	}

	// 캐릭터 조작 모드로 되돌림
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
}

void AKTSafe::BeginPlay()
{
	Super::BeginPlay();
	
	// 블루프린트에서 맞춰놓은 닫힌 문의 회전값을 저장
	ClosedDoorRotation = DoorPivot->GetRelativeRotation();
}

