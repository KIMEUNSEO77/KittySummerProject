#include "Vision/KTSpectralTargetComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Vision/KTSpectralVisionSubsystem.h"

UKTSpectralTargetComponent::UKTSpectralTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKTSpectralTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheHighlightMeshes();

	// 게임 시작 시 기본 상태는 비활성화입니다.
	bHighlightRequested = false;
	ApplyHighlightState();

	if (UWorld* World = GetWorld())
	{
		if (UKTSpectralVisionSubsystem* VisionSubsystem =
			World->GetSubsystem<
				UKTSpectralVisionSubsystem>())
		{
			VisionSubsystem->RegisterTarget(this);
		}
	}
}

void UKTSpectralTargetComponent::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UKTSpectralVisionSubsystem* VisionSubsystem =
			World->GetSubsystem<
				UKTSpectralVisionSubsystem>())
		{
			VisionSubsystem->UnregisterTarget(this);
		}
	}

	bHighlightRequested = false;
	ApplyHighlightState();

	CachedHighlightMeshes.Reset();

	Super::EndPlay(EndPlayReason);
}

void UKTSpectralTargetComponent::SetHighlightEnabled(
	const bool bEnabled)
{
	bHighlightRequested = bEnabled;
	ApplyHighlightState();
}

void UKTSpectralTargetComponent::SetTargetType(
	const EKTSpectralTargetType NewType)
{
	if (TargetType == NewType)
	{
		return;
	}

	TargetType = NewType;

	// Vision이 켜진 상태에서 종류가 바뀌면
	// 새로운 Stencil 값을 즉시 반영합니다.
	ApplyHighlightState();
}

uint8 UKTSpectralTargetComponent::GetStencilValue() const
{
	switch (TargetType)
	{
	case EKTSpectralTargetType::Guard:
		return KTSpectralStencil::Guard;

	case EKTSpectralTargetType::PrimaryTarget:
		return KTSpectralStencil::PrimaryTarget;

	case EKTSpectralTargetType::Interactable:
		return KTSpectralStencil::Interactable;

	case EKTSpectralTargetType::None:
	default:
		return KTSpectralStencil::None;
	}
}

void UKTSpectralTargetComponent::ApplyHighlightState()
{
	const uint8 StencilValue = GetStencilValue();

	const bool bShouldEnable =
		bHighlightRequested &&
		bCanBeHighlighted &&
		StencilValue != KTSpectralStencil::None;

	bHighlightEnabled = bShouldEnable;

	// 캐시가 비어 있으면 한 번 더 Mesh를 찾습니다.
	// 런타임에 Mesh가 추가된 경우를 일부 대응합니다.
	if (CachedHighlightMeshes.IsEmpty())
	{
		CacheHighlightMeshes();
	}

	for (const TWeakObjectPtr<UPrimitiveComponent>& MeshPtr :
		CachedHighlightMeshes)
	{
		UPrimitiveComponent* Mesh = MeshPtr.Get();

		if (!IsValid(Mesh))
		{
			continue;
		}

		if (bShouldEnable)
		{
			Mesh->SetCustomDepthStencilValue(
				StencilValue
			);

			Mesh->SetRenderCustomDepth(true);
		}
		else
		{
			Mesh->SetRenderCustomDepth(false);
		}

		Mesh->MarkRenderStateDirty();
	}
}

void UKTSpectralTargetComponent::CacheHighlightMeshes()
{
	CachedHighlightMeshes.Reset();

	// 에디터에서 명시적으로 지정한 Mesh가 있다면
	// 자동 탐색보다 해당 목록을 우선합니다.
	if (!HighlightMeshes.IsEmpty())
	{
		for (UPrimitiveComponent* Mesh : HighlightMeshes)
		{
			if (!IsValid(Mesh))
			{
				continue;
			}

			if (!IsSupportedHighlightMesh(Mesh))
			{
				continue;
			}

			CachedHighlightMeshes.AddUnique(Mesh);
		}

		return;
	}

	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}

	TArray<UActorComponent*> OwnerComponents;
	Owner->GetComponents(OwnerComponents);

	for (UActorComponent* ActorComponent :
		OwnerComponents)
	{
		UPrimitiveComponent* PrimitiveComponent =
			Cast<UPrimitiveComponent>(ActorComponent);

		if (!IsValid(PrimitiveComponent))
		{
			continue;
		}

		if (!IsSupportedHighlightMesh(
			PrimitiveComponent))
		{
			continue;
		}

		CachedHighlightMeshes.AddUnique(
			PrimitiveComponent
		);
	}
}

bool UKTSpectralTargetComponent::IsSupportedHighlightMesh(
	const UPrimitiveComponent* Component) const
{
	if (!IsValid(Component))
	{
		return false;
	}

	return Component->IsA<USkeletalMeshComponent>() ||
		Component->IsA<UStaticMeshComponent>();
}