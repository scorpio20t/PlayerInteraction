// Copyright by LG7, 2024

#include "Components/PlayerInteractionComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/InteractiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerInteractionComponent::TryInteract()
{
	//Putting it before any check, so that hold interaction will begin seamlessly
	// when we approach interactive objects while already holding the Interact button down
	bIsHoldInteractInProgress = true;
	
	if (!CurrentlyDetectedComponent || InteractionState != EInteractionState::OBJECT_IN_RANGE)
	{
		return;
	}

	if (!CurrentlyDetectedComponent->IsHoldInteraction())
	{
		CurrentlyDetectedComponent->OnInteract(OwningCharacter.Get());
	}
	else
	{
		if (CurrentlyDetectedComponent)
		{
			CurrentlyDetectedComponent->UpdateInteractionWidgetProgress(0.01);
		}
	}
}

void UPlayerInteractionComponent::StopHoldingInteract()
{
	bIsHoldInteractInProgress = false;
	CurrentHoldTime = 0.f;

	if (CurrentlyDetectedComponent)
	{
		CurrentlyDetectedComponent->UpdateInteractionWidgetProgress(0);
	}
}

void UPlayerInteractionComponent::TryUpdateHoldInteraction(float DeltaTime)
{
	if (!bIsHoldInteractInProgress)
	{
		return;
	}

	if (!CurrentlyDetectedComponent || !CurrentlyDetectedComponent->IsHoldInteraction() || InteractionState != EInteractionState::OBJECT_IN_RANGE)
	{
		return;
	}

	CurrentHoldTime += DeltaTime;

	CurrentlyDetectedComponent->UpdateInteractionWidgetProgress(CurrentHoldTime);

	if (GetCurrentInteractionHoldTime() <= CurrentHoldTime)
	{
		CurrentlyDetectedComponent->OnInteract(OwningCharacter.Get());
		bIsHoldInteractInProgress = false;
		CurrentHoldTime = 0.f;
	}
}

float UPlayerInteractionComponent::GetCurrentInteractionHoldTime() const
{
	if (CurrentlyDetectedComponent)
	{
		return CurrentlyDetectedComponent->HoldTime;
	}

	return 0.0f;
}

void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	OwningCharacter = Cast<ACharacter>(Owner);

	CameraComponent = OwningCharacter->FindComponentByClass<UCameraComponent>();
	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
}

void UPlayerInteractionComponent::CheckInteractionTrace()
{
	FHitResult Hit;

	FVector TraceEnd = CameraComponent->GetComponentLocation() + CameraComponent->GetForwardVector() * InteractionDetectionRange;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(Owner.Get());

	if (GetWorld()->LineTraceSingleByChannel(Hit, CameraComponent->GetComponentLocation(), TraceEnd, InteractionChannel, CollisionQueryParams) && Hit.GetActor())
	{
		UInteractiveComponent* InteractiveComponent = FindInteractiveComponentForCollision(Hit.GetActor(), Hit.GetComponent());

		if (InteractiveComponent)
		{
			if (InteractiveComponent != CurrentlyDetectedComponent)
			{
				if (CurrentlyDetectedComponent.Get())
				{
					ClearDetectedComponent();
				}

				CurrentlyDetectedComponent = InteractiveComponent;
			}
		}
		else
		{
			if (CurrentlyDetectedComponent)
			{
				ClearDetectedComponent();
			}
		}
	}

	else
	{
		if (CurrentlyDetectedComponent)
		{
			ClearDetectedComponent();
		}
	}
}

float UPlayerInteractionComponent::GetDistanceFromDetectedComponent() const
{
	return FVector::Distance(CameraComponent->GetComponentLocation(), CurrentlyDetectedComponent->GetInteractionCollision()->GetComponentLocation());
}

void UPlayerInteractionComponent::CheckDistanceFromDetectedComponent()
{
	if (GetDistanceFromDetectedComponent() < InteractionRange && InteractionState != EInteractionState::OBJECT_IN_RANGE)
	{
		InteractionState = EInteractionState::OBJECT_IN_RANGE;
		CurrentlyDetectedComponent->OnDetected(OwningCharacter.Get(), true);
	}
	else if (GetDistanceFromDetectedComponent() > InteractionRange && InteractionState != EInteractionState::OBJECT_DETECTED)
	{
		InteractionState = EInteractionState::OBJECT_DETECTED;
		CurrentlyDetectedComponent->OnDetected(OwningCharacter.Get(), false);
	}
}

void UPlayerInteractionComponent::ClearDetectedComponent()
{
	InteractionState = EInteractionState::NO_OBJECT;
	CurrentlyDetectedComponent->OnLost(OwningCharacter.Get());
	CurrentlyDetectedComponent = nullptr;
}

UInteractiveComponent* UPlayerInteractionComponent::FindInteractiveComponentForCollision(AActor* HitActor, UPrimitiveComponent* Collision)
{
	TInlineComponentArray<UInteractiveComponent*> InteractiveComponents;
	HitActor->GetComponents<UInteractiveComponent>(InteractiveComponents);

	if (InteractiveComponents.Num() == 0)
	{
		return nullptr;
	}

	for (auto it : InteractiveComponents)
	{
		if (it->GetInteractionCollision() == Collision)
		{
			return it;
		}
	}

	return nullptr;
}

void UPlayerInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bFullCameraFadeDisablesInteraction || PlayerCameraManager.Get()->FadeAmount < 1.f)
	{
		CheckInteractionTrace();

		if (CurrentlyDetectedComponent)
		{
			CheckDistanceFromDetectedComponent();
			TryUpdateHoldInteraction(DeltaTime);
		}
	}
}