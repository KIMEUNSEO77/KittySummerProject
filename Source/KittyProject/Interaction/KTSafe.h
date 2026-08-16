// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/KTInteractableInterface.h"
#include "KTSafe.generated.h"

class AKTItemPickupBase;

UCLASS()
class KITTYPROJECT_API AKTSafe : public AActor, public IKTInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKTSafe();
	
	// F키를 눌렀을 때 실행되는 상호작용
	virtual void Interact_Implementation(AActor* Interactor) override;

	// 금고를 바라봤을 때 표시할 안내 문구
	virtual FText GetInteractionText_Implementation() const override;
	
	// 매 프레임 금고 문을 부드럽게 회전
	virtual void Tick(float DeltaTime) override;

	// 키패드에서 입력한 비밀번호를 검사
	bool TryUnlock(const FString& EnteredPassword);

	// 키패드를 닫고 캐릭터 조작으로 복귀
	void CloseKeypad();
	
protected:
	// 모든 컴포넌트의 기준점
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Safe|Components")
	TObjectPtr<class USceneComponent> SceneRoot;

	// 움직이지 않는 금고 본체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Safe|Components")
	TObjectPtr<class UStaticMeshComponent> SafeBodyMesh;

	// 금고 문 회전의 기준점
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Safe|Components")
	TObjectPtr<class USceneComponent> DoorPivot;

	// 열고 닫을 금고 문
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Safe|Components")
	TObjectPtr<class UStaticMeshComponent> SafeDoorMesh;

	// 플레이어의 상호작용 Line Trace가 감지할 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Safe|Components")
	TObjectPtr<class USphereComponent> InteractionCollision;

	// 블루프린트에서 변경할 수 있는 정답
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Safe")
	FString CorrectPassword = TEXT("0404");

	// 금고가 열렸는지 확인
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Safe")
	bool bIsOpen = false;
	
	// 화면에 띄울 금고 키패드 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Safe|UI")
	TSubclassOf<class UKTSafeKeypadWidget> SafeKeypadWidgetClass;

	// 현재 화면에 생성된 키패드 위젯
	UPROPERTY()
	TObjectPtr<class UKTSafeKeypadWidget> SafeKeypadWidget;
	
	virtual void BeginPlay() override;

	// 금고 문이 열릴 각도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Safe|Door")
	float OpenAngle = 90.0f;

	// 숫자가 클수록 문이 빠르게 열림
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Safe|Door")
	float DoorOpenSpeed = 2.0f;

	// 현재 문이 열리는 중인지
	bool bIsOpening = false;

	// 블루프린트에서 설정한 문의 최초 회전값
	FRotator ClosedDoorRotation;
	
	// 금고가 열린 뒤 획득할 아이템
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Safe|Reward")
	TObjectPtr<AKTItemPickupBase> RewardPickup;
};
