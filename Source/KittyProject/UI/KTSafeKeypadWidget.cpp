// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KTSafeKeypadWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/Engine.h"
#include "Interaction/KTSafe.h"

void UKTSafeKeypadWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Btn_0->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber0);
	Btn_1->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber1);
	Btn_2->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber2);
	Btn_3->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber3);
	Btn_4->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber4);
	Btn_5->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber5);
	Btn_6->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber6);
	Btn_7->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber7);
	Btn_8->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber8);
	Btn_9->OnClicked.AddDynamic(this, &UKTSafeKeypadWidget::HandleNumber9);

	Btn_Clear->OnClicked.AddDynamic(
		this,
		&UKTSafeKeypadWidget::HandleClear
	);

	Btn_Confirm->OnClicked.AddDynamic(
		this,
		&UKTSafeKeypadWidget::HandleConfirm
	);

	EnteredPassword.Empty();
	UpdatePasswordText();
}

void UKTSafeKeypadWidget::AddNumber(const FString& Number)
{
	// 비밀번호는 최대 4자리까지만 입력
	if (EnteredPassword.Len() >= 4)
	{
		return;
	}

	EnteredPassword += Number;
	UpdatePasswordText();
}

void UKTSafeKeypadWidget::UpdatePasswordText()
{
	if (!Txt_EnteredPassword)
	{
		return;
	}

	FString DisplayText = EnteredPassword;

	// 아직 입력하지 않은 자리는 밑줄로 표시
	while (DisplayText.Len() < 4)
	{
		DisplayText += TEXT("_");
	}

	Txt_EnteredPassword->SetText(
		FText::FromString(DisplayText)
	);
}

void UKTSafeKeypadWidget::HandleNumber0()
{
	AddNumber(TEXT("0"));
}

void UKTSafeKeypadWidget::HandleNumber1()
{
	AddNumber(TEXT("1"));
}

void UKTSafeKeypadWidget::HandleNumber2()
{
	AddNumber(TEXT("2"));
}

void UKTSafeKeypadWidget::HandleNumber3()
{
	AddNumber(TEXT("3"));
}

void UKTSafeKeypadWidget::HandleNumber4()
{
	AddNumber(TEXT("4"));
}

void UKTSafeKeypadWidget::HandleNumber5()
{
	AddNumber(TEXT("5"));
}

void UKTSafeKeypadWidget::HandleNumber6()
{
	AddNumber(TEXT("6"));
}

void UKTSafeKeypadWidget::HandleNumber7()
{
	AddNumber(TEXT("7"));
}

void UKTSafeKeypadWidget::HandleNumber8()
{
	AddNumber(TEXT("8"));
}

void UKTSafeKeypadWidget::HandleNumber9()
{
	AddNumber(TEXT("9"));
}

void UKTSafeKeypadWidget::HandleClear()
{
	EnteredPassword.Empty();
	UpdatePasswordText();
}

void UKTSafeKeypadWidget::HandleConfirm()
{
	// 네 자리 모두 입력하지 않았다면 판정하지 않음
	if (EnteredPassword.Len() != 4)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Red,
				TEXT("비밀번호 네 자리를 입력하세요.")
			);
		}

		return;
	}

	if (!IsValid(OwningSafe))
	{
		return;
	}

	// 정답이면 TryUnlock 내부에서 문을 열고 UI 종료
	if (OwningSafe->TryUnlock(EnteredPassword))
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			TEXT("비밀번호가 일치하지 않습니다.")
		);
	}

	// 오답이면 입력 내용 삭제
	ResetPassword();
}

void UKTSafeKeypadWidget::SetOwningSafe(AKTSafe* InSafe)
{
	OwningSafe = InSafe;
}

void UKTSafeKeypadWidget::ResetPassword()
{
	EnteredPassword.Empty();
	UpdatePasswordText();
}
