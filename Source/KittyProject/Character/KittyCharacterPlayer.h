// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/KittyCharacterBase.h"
#include "InputActionValue.h"
#include "Animation/AnimInstance.h"
#include "KittyCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API AKittyCharacterPlayer : public AKittyCharacterBase
{
	GENERATED_BODY()
	
public:
	AKittyCharacterPlayer();
	
	// 플레이어가 가진 인벤토리를 반환합니다.
	class UKTInventoryComponent* GetInventoryComponent() const
	{
		return InventoryComponent;
	}
	
	void EnterInventoryCamera();
    void ExitInventoryCamera();
    
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Character Control Section
protected:
	void ChangeCharacterControl();
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	virtual void SetCharacterControlData(const class UKittyCharacterControlData* CharacterControlData) override;	
	
	// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;
	
	// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeControlAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> QuaterMoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MouseLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> MouseLookMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;
	
	// 상호작용 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractionAction;
	
	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);
	
	void QuaterMove(const FInputActionValue& Value);
	
	ECharacterControlType CurrentCharacterControlType;
	
	// Animation Section
public:
	void JumpStart();
	void JumpEnd();
	bool bIsJumping = false;

	void CrouchStart();
	void CrouchEnd();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch|Animation")
	TObjectPtr<class UAnimSequence> StandingToCrouchedAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch|Animation")
	TObjectPtr<class UAnimSequence> CrouchedToStandingAnimation;
	
	// Interaction Section
protected:
	UPROPERTY(
	EditDefaultsOnly,
	BlueprintReadOnly,
	Category = "Interaction|Sound"
)
	TObjectPtr<class USoundBase> InteractionSound;
	
	// 매 프레임 플레이어가 바라보는 상호작용 대상을 검사
	void CheckForInteractable();

	// 상호작용 가능한 최대 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance = 300.0f;

	// 현재 플레이어가 바라보고 있는 상호작용 대상
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<class AActor> CurrentInteractable;
	
	// 현재 감지된 Actor와 상호작용
	void Interact();
	void ToggleInventory();
	
	// Pistol Section
public:
	// 권총 Actor를 플레이어 손에 장착
	bool AcquirePistol(class AActor* PistolActor);
	
	bool IsAiming() const { return bIsAiming;}
	
protected:
	// 권총 보유 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bHasPistol = false;

	// 현재 장착된 권총
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class AActor> EquippedPistol;
	
	void StartAiming();
	void StopAiming();
	void Fire();
	void StopFiring();
	void PerformFireTrace();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsAiming = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsFiring = false;
	
	// 발사 애니메이션이 유지될 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	float FireAnimationDuration = 0.3f;

	FTimerHandle FireAnimationTimerHandle;
	
	// 권총의 최대 사거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireRange = 10000.0f;
	
	// 카드키, 문서, 조사물 등을 저장하는 인벤토리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class UKTInventoryComponent> InventoryComponent;
	
	// 권총 발사 시 재생할 카메라 흔들림
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Camera")
	TSubclassOf<class UCameraShakeBase> FireCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Camera")
	float FireCameraShakeScale = 1.0f;
	
	// 인벤토리 진입 카메라
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Camera",
		meta = (AllowPrivateAccess = "true"))
	float InventoryCameraArmLength = 250.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Camera",
		meta = (AllowPrivateAccess = "true"))
	FVector InventoryCameraSocketOffset = FVector(0.0f, 125.0f, 35.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Camera",
		meta = (AllowPrivateAccess = "true"))
	float InventoryCameraFOV = 55.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Camera",
		meta = (AllowPrivateAccess = "true"))
	float InventoryCameraTransitionDuration = 0.2f;

	bool bIsInventoryCameraActive = false;
	bool bIsInventoryCameraTransitioning = false;
	
	float InventoryCameraTransitionElapsed = 0.0f;

	float NormalCameraArmLength = 0.0f;
	FVector NormalCameraSocketOffset = FVector::ZeroVector;
	float NormalCameraFOV = 90.0f;

	float CameraTransitionStartArmLength = 0.0f;
	FVector CameraTransitionStartSocketOffset = FVector::ZeroVector;
	float CameraTransitionStartFOV = 90.0f;

	void UpdateInventoryCamera(float DeltaTime);
	
	// Interaction Animation Section
public:
	// 총 획득 애니메이션 시작
	void StartPistolPickup(class AKTPistolPickup* PistolPickup);
	
	// 일반 아이템 획득 애니메이션 시작
	void StartItemPickup(class AKTItemPickupBase* ItemPickup, class UAnimMontage* ItemMontage);
	
	// 출입증 사용 애니메이션 시작
	void StartKeycardDoorInteraction(class AKTKeycardDoor* KeycardDoor);
	
	// 통신 단말기 조작 애니메이션 시작
	void StartTerminalInteraction(class AKTCommunicationTerminal* Terminal);
	
protected:
	// 상호작용 애니메이션 실행 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bIsPerformingInteraction = false;

	// 총 획득 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Animation")
	TObjectPtr<class UAnimMontage> PistolPickupMontage;

	// 애니메이션이 끝날 때까지 기억할 총
	UPROPERTY()
	TObjectPtr<class AKTPistolPickup> PendingPistolPickup;

	// 상호작용 중 조작 잠금
	void BeginInteractionLock();

	// 상호작용 종료 후 조작 해제
	void EndInteractionLock();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction|MotionWarping")
	TObjectPtr<class UMotionWarpingComponent> MotionWarpingComponent;
	
	// 몽타주에서 Montage Notify가 발생했을 때 호출
	UFUNCTION()
	void HandleInteractionNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	// 몽타주가 정상 종료되거나 중단됐을 때 호출
	UFUNCTION()
	void HandleInteractionMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	// 현재 획득 애니메이션을 실행 중인 일반 아이템
	UPROPERTY()
	TObjectPtr<class AKTItemPickupBase> PendingItemPickup;

	// 현재 실행 중인 일반 아이템 몽타주
	UPROPERTY()
	TObjectPtr<class UAnimMontage> ActiveItemPickupMontage;
	
	// 출입증 사용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Animation")
	TObjectPtr<class UAnimMontage> KeycardUseMontage;

	// 애니메이션이 끝날 때까지 기억할 출입증 문
	UPROPERTY()
	TObjectPtr<class AKTKeycardDoor> PendingKeycardDoor;
	
	// 출입증 사용 연출 동안 손에 표시할 카드키 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction|Keycard")
	TObjectPtr<class UStaticMeshComponent> KeycardUseMesh;
	
	// 통신 단말기 조작 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Animation")
	TObjectPtr<class UAnimMontage> TerminalInteractionMontage;

	// 애니메이션이 끝날 때까지 기억할 통신 단말기
	UPROPERTY()
	TObjectPtr<class AKTCommunicationTerminal> PendingTerminal;
};
