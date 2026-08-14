// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KTSafeKeypadWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/Engine.h"

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
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Yellow,
			FString::Printf(
				TEXT("입력한 비밀번호: %s"),
				*EnteredPassword
			)
		);
	}
}