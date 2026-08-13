#include "UI/KTObjectiveMarkerWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"

void UKTObjectiveMarkerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
	SetVisibility(ESlateVisibility::Collapsed);

	UpdateDisplayMode();
}

void UKTObjectiveMarkerWidget::SetTargetActor(
	AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
	InteractionActor.Reset();

	// 새 목적지가 생기면 우선 일반 목적지 마커 상태로 시작합니다.
	bShowingInteractionPrompt = false;
	UpdateDisplayMode();

	SetVisibility(
		TargetActor.IsValid()
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed
	);
}

bool UKTObjectiveMarkerWidget::IsTrackingActor(
	const AActor* Actor) const
{
	return IsValid(Actor) &&
		TargetActor.IsValid() &&
		TargetActor.Get() == Actor;
}

void UKTObjectiveMarkerWidget::SetInteractionPrompt(
	AActor* InteractableActor,
	const FText& PromptText)
{
	InteractionActor = InteractableActor;
	bShowingInteractionPrompt = InteractionActor.IsValid();

	if (Txt_InteractionText &&
		bShowingInteractionPrompt)
	{
		Txt_InteractionText->SetText(PromptText);
	}

	UpdateDisplayMode();

	SetVisibility(
		bShowingInteractionPrompt || TargetActor.IsValid()
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed
	);
}

void UKTObjectiveMarkerWidget::UpdateDisplayMode()
{
	if (Panel_Marker)
	{
		Panel_Marker->SetVisibility(
			bShowingInteractionPrompt
				? ESlateVisibility::Collapsed
				: ESlateVisibility::HitTestInvisible
		);
	}

	if (Panel_Interaction)
	{
		Panel_Interaction->SetVisibility(
			bShowingInteractionPrompt
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed
		);
	}
}

void UKTObjectiveMarkerWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateMarker();
}

void UKTObjectiveMarkerWidget::UpdateMarker()
{
	AActor* DisplayActor =
		bShowingInteractionPrompt && InteractionActor.IsValid()
			? InteractionActor.Get()
			: TargetActor.Get();

	if (!IsValid(DisplayActor))
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	APlayerController* PlayerController = GetOwningPlayer();

	if (!PlayerController ||
		!PlayerController->PlayerCameraManager ||
		!GEngine ||
		!GEngine->GameViewport)
	{
		return;
	}

	const FVector TargetLocation =
		DisplayActor->GetActorLocation() +
		FVector(0.0f, 0.0f, 50.0f);

	const FVector CameraLocation =
		PlayerController->PlayerCameraManager->GetCameraLocation();

	const FRotator CameraRotation =
		PlayerController->PlayerCameraManager->GetCameraRotation();

	const FVector DirectionToTarget =
		(TargetLocation - CameraLocation).GetSafeNormal();

	const FVector CameraForward = CameraRotation.Vector();

	const FVector CameraRight =
		FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);

	const float ForwardDot =
		FVector::DotProduct(CameraForward, DirectionToTarget);

	const float RightDot =
		FVector::DotProduct(CameraRight, DirectionToTarget);

	const float ViewportScale =
		UWidgetLayoutLibrary::GetViewportScale(this);

	const FVector2D ViewportSize =
		UWidgetLayoutLibrary::GetViewportSize(this) /
		ViewportScale;

	FVector2D ScreenPosition;

	const bool bProjected =
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
			PlayerController,
			TargetLocation,
			ScreenPosition,
			false
		);

	const float EdgePadding = 60.0f;

	const bool bIsOnScreen =
		bProjected &&
		ForwardDot > 0.0f &&
		ScreenPosition.X > EdgePadding &&
		ScreenPosition.X < ViewportSize.X - EdgePadding &&
		ScreenPosition.Y > EdgePadding &&
		ScreenPosition.Y < ViewportSize.Y - EdgePadding;

	float ArrowAngle = 0.0f;

	if (bIsOnScreen)
	{
		SetPositionInViewport(ScreenPosition, false);
	}
	else
	{
		const float AngleRadians =
			FMath::Atan2(RightDot, ForwardDot);

		const FVector2D Direction2D(
			FMath::Sin(AngleRadians),
			-FMath::Cos(AngleRadians)
		);

		const FVector2D ViewportCenter =
			ViewportSize * 0.5f;

		const FVector2D HalfSize =
			(ViewportSize * 0.5f) -
			FVector2D(EdgePadding, EdgePadding);

		const float XScale =
			FMath::Abs(Direction2D.X) >
				KINDA_SMALL_NUMBER
				? HalfSize.X / FMath::Abs(Direction2D.X)
				: BIG_NUMBER;

		const float YScale =
			FMath::Abs(Direction2D.Y) >
				KINDA_SMALL_NUMBER
				? HalfSize.Y / FMath::Abs(Direction2D.Y)
				: BIG_NUMBER;

		const float ClampScale =
			FMath::Min(XScale, YScale);

		const FVector2D ClampedPosition =
			ViewportCenter + Direction2D * ClampScale;

		SetPositionInViewport(ClampedPosition, false);

		ArrowAngle =
			FMath::RadiansToDegrees(AngleRadians);
	}

	if (Img_Arrow)
	{
		Img_Arrow->SetRenderTransformAngle(
			ArrowAngle
		);
	}

	// 상호작용 중에는 거리 텍스트를 숨깁니다.
	if (Txt_Distance)
	{
		Txt_Distance->SetVisibility(
			bShowingInteractionPrompt
				? ESlateVisibility::Collapsed
				: ESlateVisibility::Visible
		);

		const float DistanceCm = FVector::Distance(
			PlayerController->GetPawn()
				? PlayerController->GetPawn()->GetActorLocation()
				: CameraLocation,
			TargetLocation
		);

		const int32 DistanceM =
			FMath::RoundToInt(DistanceCm / 100.0f);

		Txt_Distance->SetText(
			FText::FromString(
				FString::Printf(TEXT("%dm"), DistanceM)
			)
		);
	}
}
