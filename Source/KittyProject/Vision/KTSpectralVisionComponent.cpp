#include "Vision/KTSpectralVisionComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Vision/KTSpectralVisionSubsystem.h"
#include "Misc/App.h"
#include "Kismet/GameplayStatics.h"
#include "Character/KittyCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/KittyPlayerController.h"

UKTSpectralVisionComponent::UKTSpectralVisionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UKTSpectralVisionComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(
        DeltaTime,
        TickType,
        ThisTickFunction
    );

    UpdateVisionTransition();

    if (bVisionEnabled)
    {
        UpdateDepthOfField();
    }
}

void UKTSpectralVisionComponent::BeginPlay()
{
    Super::BeginPlay();

    bVisionEnabled = false;

    if (
        UKTSpectralVisionSubsystem* VisionSubsystem =
            GetVisionSubsystem()
    )
    {
        VisionSubsystem->SetVisionEnabled(false);
    }
}

void UKTSpectralVisionComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    DeactivateVision();

    Super::EndPlay(EndPlayReason);
}

void UKTSpectralVisionComponent::ToggleVision()
{
    if (bVisionEnabled)
    {
        DeactivateVision();
    }
    else
    {
        ActivateVision();
    }
}

void UKTSpectralVisionComponent::ActivateVision()
{
    if (bVisionEnabled ||
       !CanActivateVision())
    {
        return;
    }

    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    USpringArmComponent* CameraBoom =
        Player->GetCameraBoom();

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(CameraBoom) ||
        !IsValid(FollowCamera))
    {
        return;
    }

    UKTSpectralVisionSubsystem* VisionSubsystem =
        GetVisionSubsystem();

    if (!IsValid(VisionSubsystem))
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!IsValid(World))
    {
        return;
    }

    // Spectral Vision 진입 전 현재 카메라 상태 저장
    SavedCameraArmLength =
        CameraBoom->TargetArmLength;

    SavedCameraSocketOffset =
        CameraBoom->SocketOffset;

    SavedCameraFOV =
        FollowCamera->FieldOfView;

    // 진입 애니메이션 시작
    VisionTransitionAlpha = 0.0f;
    VisionTransitionDirection = 1.0f;

    bVisionEnabled = true;
    
    // 흑백 Post Process 적용
    ApplyVisionVisuals();
    
    // 현재 시간 배율을 저장한 후 슬로모션 적용
    PreviousTimeDilation =
        UGameplayStatics::GetGlobalTimeDilation(this);

    UGameplayStatics::SetGlobalTimeDilation(
        this,
        VisionTimeDilation
    );

    // 현재 플레이어 위치를 기준으로 범위 검사
    VisionSubsystem->UpdateTargetsInRange(
        Player->GetActorLocation(),
        HighlightRange
    );

    VisionSubsystem->SetVisionEnabled(true);

    World->GetTimerManager().SetTimer(
        TargetRefreshTimerHandle,
        this,
        &UKTSpectralVisionComponent::RefreshTargets,
        TargetRefreshInterval,
        true
    );
}

void UKTSpectralVisionComponent::DeactivateVision()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(
            TargetRefreshTimerHandle
        );
    }

    if (!bVisionEnabled)
    {
        return;
    }

    bVisionEnabled = false;
    // DOF 설정 복구
    RestoreDepthOfField();
    
    // 흑백 화면을 기존 컬러 설정으로 복구
    RestoreVisionVisuals();
    
    // Spectral Vision을 켜기 전 시간 배율로 복구
    UGameplayStatics::SetGlobalTimeDilation(
        this,
        PreviousTimeDilation
    );

    if (UKTSpectralVisionSubsystem* VisionSubsystem =
        GetVisionSubsystem())
    {
        VisionSubsystem->SetVisionEnabled(false);
    }
}

void UKTSpectralVisionComponent::RefreshTargets()
{
    if (!bVisionEnabled)
    {
        return;
    }

    const AActor* Owner = GetOwner();

    if (!IsValid(Owner))
    {
        DeactivateVision();
        return;
    }

    if (
        UKTSpectralVisionSubsystem* VisionSubsystem =
            GetVisionSubsystem()
    )
    {
        VisionSubsystem->UpdateTargetsInRange(
            Owner->GetActorLocation(),
            HighlightRange
        );
    }
}

UKTSpectralVisionSubsystem*
UKTSpectralVisionComponent::GetVisionSubsystem() const
{
    const UWorld* World = GetWorld();

    if (!IsValid(World))
    {
        return nullptr;
    }

    return World->GetSubsystem<
        UKTSpectralVisionSubsystem
    >();
}

void UKTSpectralVisionComponent::UpdateVisionTransition()
{
    const float RealDeltaTime = FApp::GetDeltaTime();
    (void)RealDeltaTime;
}

AKittyCharacterPlayer*
UKTSpectralVisionComponent::GetPlayerCharacter() const
{
    return Cast<AKittyCharacterPlayer>(
        GetOwner()
    );
}

bool UKTSpectralVisionComponent::CanActivateVision() const
{
    const AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return false;
    }

    const AKittyPlayerController* Controller =
        Cast<AKittyPlayerController>(
            Player->GetController()
        );

    if (!IsValid(Controller))
    {
        return false;
    }

    return
        !Controller->IsInventoryOpen() &&
        !Controller->IsExamineOpen() &&
        !Controller->IsGameplayUIHidden() &&
        !Controller->IsRadioInputLocked();
}


void UKTSpectralVisionComponent::UpdateDepthOfField()
{
    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(FollowCamera))
    {
        return;
    }

    const FVector CameraLocation =
        FollowCamera->GetComponentLocation();

    const FVector ChestLocation =
        Player->GetActorLocation() +
        FVector(0.0f, 0.0f, 60.0f);

    const float FocusDistance =
        FVector::Distance(
            CameraLocation,
            ChestLocation
        );

    FPostProcessSettings& Settings =
        FollowCamera->PostProcessSettings;

    Settings.bOverride_DepthOfFieldFstop = true;
    Settings.DepthOfFieldFstop = VisionFStop;

    Settings.bOverride_DepthOfFieldFocalDistance = true;
    Settings.DepthOfFieldFocalDistance = FocusDistance;

    Settings.bOverride_DepthOfFieldFocalRegion = true;
    Settings.DepthOfFieldFocalRegion =
        VisionFocalRegion;

    Settings.bOverride_DepthOfFieldNearTransitionRegion =
        true;

    Settings.DepthOfFieldNearTransitionRegion =
        150.0f;

    Settings.bOverride_DepthOfFieldFarTransitionRegion =
        true;

    Settings.DepthOfFieldFarTransitionRegion =
        400.0f;

    // 카메라 Post Process 설정이 화면에 적용되도록 설정
    FollowCamera->PostProcessBlendWeight = 1.0f;
}

void UKTSpectralVisionComponent::RestoreDepthOfField()
{
    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(FollowCamera))
    {
        return;
    }

    FPostProcessSettings& Settings =
        FollowCamera->PostProcessSettings;

    Settings.bOverride_DepthOfFieldFstop = false;

    Settings.bOverride_DepthOfFieldFocalDistance =
        false;

    Settings.bOverride_DepthOfFieldFocalRegion =
        false;

    Settings.bOverride_DepthOfFieldNearTransitionRegion =
        false;

    Settings.bOverride_DepthOfFieldFarTransitionRegion =
        false;
}


void UKTSpectralVisionComponent::ApplyVisionVisuals()
{
    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(FollowCamera))
    {
        return;
    }

    FPostProcessSettings& Settings =
        FollowCamera->PostProcessSettings;

    // 최초 활성화 시에만 기존 설정을 저장합니다.
    // 같은 설정을 여러 번 덮어쓰지 않도록 보호합니다.
    if (!bHasSavedVisualSettings)
    {
        bSavedOverrideColorSaturation =
            Settings.bOverride_ColorSaturation;

        SavedColorSaturation =
            Settings.ColorSaturation;

        SavedPostProcessBlendWeight =
            FollowCamera->PostProcessBlendWeight;

        bHasSavedVisualSettings = true;
    }

    // 카메라가 Color Saturation 값을 사용하도록 설정합니다.
    Settings.bOverride_ColorSaturation = true;

    // RGB 채도를 0으로 만들면 흑백이 됩니다.
    // 마지막 W는 1로 유지합니다.
    Settings.ColorSaturation =
        FVector4(
            0.0f,
            0.0f,
            0.0f,
            1.0f
        );

    // 카메라 Post Process를 화면에 완전히 적용합니다.
    FollowCamera->PostProcessBlendWeight = 1.0f;
}

void UKTSpectralVisionComponent::RestoreVisionVisuals()
{
    if (!bHasSavedVisualSettings)
    {
        return;
    }

    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(FollowCamera))
    {
        return;
    }

    FPostProcessSettings& Settings =
        FollowCamera->PostProcessSettings;

    // Spectral Vision 진입 전에 사용하던 설정으로 복원합니다.
    Settings.bOverride_ColorSaturation =
        bSavedOverrideColorSaturation;

    Settings.ColorSaturation =
        SavedColorSaturation;

    FollowCamera->PostProcessBlendWeight =
        SavedPostProcessBlendWeight;

    bHasSavedVisualSettings = false;
}