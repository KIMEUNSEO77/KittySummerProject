// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Attack/KTBatonAttackComponent.h"
#include "Character/KittyCharacterBase.h"
#include "Interface/KittyCharacterAIInterface.h"
#include "KittyCharacterNonplayer.generated.h"

/**
 * 
 */

class UAnimMontage;
class ATargetPoint;
class UKTBatonAttackComponent;
class UKTGunAttackComponent;


UENUM(BlueprintType)
enum class EKittyDeathDirection : uint8
{
	None  UMETA(DisplayName = "None"),
	Front UMETA(DisplayName = "Front"),
	Back  UMETA(DisplayName = "Back"),
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class EGuardWeaponType : uint8
{
	Baton UMETA(DisplayName = "Baton"),
	Gun UMETA(DisplayName = "Gun")
};

// 경비원이 사망했을 때 알리는 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGuardDiedSignature);

UCLASS()
class KITTYPROJECT_API AKittyCharacterNonplayer : public AKittyCharacterBase, public IKittyCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AKittyCharacterNonplayer();
	
	bool IsPatrolEnabled() const
	{
		return bPatrolEnabled;
	}
	
	const TArray<TObjectPtr<ATargetPoint>>& GetPatrolPoints() const
	{
		return PatrolPoints;
	}
	
	bool IsDead() const
	{
		return bIsDead;
	}
	
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void OnDamaged(AActor* DamageActor,
		float Damage, 
		const UDamageType* DamageType, 
		AController* InstigatedBy, 
		AActor* DamageCauser);

protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAITurnSpeed() override;	
	
private:
	
	//순찰가능 여부를 판단
	UPROPERTY(EditAnywhere, Category = AI)
	bool bPatrolEnabled;
	//순찰 위치를 저장하는 배열
	UPROPERTY(EditAnywhere, Category = AI)
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;
	//죽었는지 판단
	UPROPERTY(EditAnywhere, Category = "Debug|Death",meta = (AllowPrivateAccess = "true"))
	bool bIsDead;
	//죽은 상태를 디버그하기위한 변수
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category = "Debug|Death", meta = (AllowPrivateAccess = "true"))
	EKittyDeathDirection DeathDirection = EKittyDeathDirection::None;
	//각 방향별 죽는 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Death", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> BackDeathMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Death", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> FrontDeathMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Death", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> LeftDeathMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Death", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> RightDeathMontage;


private:
	//죽는 방향 선택
	EKittyDeathDirection CalculateDeathDirection(AController* InstigatedBy,AActor* DamageCauser) const;
	//죽을때 재생할 몽타주 선택
	UAnimMontage* GetDeathMontage(EKittyDeathDirection Direction) const;
	//선택된 몽타주로 죽는 모션 실행.
	void Die(UAnimMontage* DeathMontage);
	
private:
	UPROPERTY(EditAnywhere, Category = "Debug|Death")
	EKittyDeathDirection DebugDirection = EKittyDeathDirection::Front;
	
	UFUNCTION(CallInEditor, Category = "Debug|Death")
	void DebugKill();
	
	// Item Section
private:
	// 경비원이 소지한 아이템 종류
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AKTItemPickupBase> CarriedItemClass;

	// 아이템을 붙일 메시 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop", meta = (AllowPrivateAccess = "true"))
	FName CarriedItemSocketName = TEXT("KeycardSocket");

	// 허리에 붙어 있는 아이템
	UPROPERTY()
	TObjectPtr<class AKTItemPickupBase> CarriedItem;
	
public:
	UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
	void ConfigurePatrolRoute(const TArray<ATargetPoint*>& InPatrolPoints);
	
public:
	//경비원의 공격 타입 설정을 위한 섹션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Attack", meta = (AllowPrivateAccess = "true"))
	EGuardWeaponType GuardWeaponType = EGuardWeaponType::Baton;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UKTBatonAttackComponent> BatonAttackComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UKTGunAttackComponent> GunAttackComponent;

	UKTGuardAttackComponent* GetAttackComponent() const;
	
	virtual float GetAIAttackRange() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "AI|Gun")
	void FireGuardProjectile(AActor* Target, float Damage);
	
public:
	//경비원의 상태 변경을 위한 변수
	UFUNCTION(BlueprintCallable, Category = "AI|Weapon")
	void SetGuardWeaponType(EGuardWeaponType NewType);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "AI|Weapon")
	void OnGuardWeaponTypeChanged(EGuardWeaponType NewType);
	
	// CCTV Section
public:
	// 경비원 사망 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Guard|Death")
	FOnGuardDiedSignature OnGuardDied;
};
