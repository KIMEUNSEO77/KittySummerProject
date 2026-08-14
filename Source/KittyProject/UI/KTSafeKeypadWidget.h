// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KTSafeKeypadWidget.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API UKTSafeKeypadWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// 위젯이 처음 만들어졌을 때 버튼 이벤트를 연결합니다.
	virtual void NativeOnInitialized() override;

	// WBP_SafeKeypad의 위젯 이름과 정확히 같아야 합니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_EnteredPassword;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_7;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_8;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_9;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Clear;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Confirm;

private:
	// 현재 플레이어가 입력한 비밀번호
	FString EnteredPassword;

	// 숫자를 현재 비밀번호 뒤에 추가
	void AddNumber(const FString& Number);

	// 화면의 비밀번호 표시 갱신
	void UpdatePasswordText();

	UFUNCTION()
	void HandleNumber0();

	UFUNCTION()
	void HandleNumber1();

	UFUNCTION()
	void HandleNumber2();

	UFUNCTION()
	void HandleNumber3();

	UFUNCTION()
	void HandleNumber4();

	UFUNCTION()
	void HandleNumber5();

	UFUNCTION()
	void HandleNumber6();

	UFUNCTION()
	void HandleNumber7();

	UFUNCTION()
	void HandleNumber8();

	UFUNCTION()
	void HandleNumber9();

	UFUNCTION()
	void HandleClear();

	UFUNCTION()
	void HandleConfirm();
	
	// 현재 키패드를 사용 중인 금고
	UPROPERTY()
	TObjectPtr<class AKTSafe> OwningSafe;

public:
	// 현재 이 키패드를 열어 준 금고를 저장
	void SetOwningSafe(AKTSafe* InSafe);

	// 키패드를 열 때 이전 입력값 초기화
	void ResetPassword();
};
