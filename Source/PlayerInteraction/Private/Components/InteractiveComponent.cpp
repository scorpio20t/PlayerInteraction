// Copyright by LG7, 2024


#include "Components/InteractiveComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/Interface_InteractionReceiver.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Settings/PlayerInteractionSettings.h"

UInteractiveComponent::UInteractiveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;	
}

void UInteractiveComponent::OnDetected(ACharacter* Character, bool InRange)
{
	LastInteractingCharacter = Character;
	OnDetectedNotify.Broadcast(Character, InRange);
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (InteractionWidget->GetClass()->ImplementsInterface(UInterface_InteractionReceiver::StaticClass()))
		{
			IInterface_InteractionReceiver::Execute_OnInteractionDetected(InteractionWidget, Character, InRange, InteractionText);
		}
	}
}

void UInteractiveComponent::OnInteract(ACharacter* Character)
{
	UpdateInteractionWidgetProgress(0.f);
	OnInteraction.Broadcast(Character);
}

void UInteractiveComponent::OnLost(ACharacter* Character)
{
	OnLostNotify.Broadcast(Character);
	if (InteractionWidget)
	{
		UpdateInteractionWidgetProgress(0.f);
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInteractiveComponent::SetInteraction(bool Active)
{
	bActive = Active;
	if (InteractionCollision.IsValid())
	{
		if (!bActive)
		{
			InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}

void UInteractiveComponent::SetInteractionText(const FText& Text)
{
	InteractionText = Text;

	if (InteractionWidget)
	{
		if (InteractionWidget->GetClass()->ImplementsInterface(UInterface_InteractionReceiver::StaticClass()))
		{
			IInterface_InteractionReceiver::Execute_UpdateInteractionText(InteractionWidget, InteractionText);
		}
	}
}

void UInteractiveComponent::SetHoldToInteract(bool bShouldHoldToInteract)
{
	bHoldToInteract = bShouldHoldToInteract;
}

bool UInteractiveComponent::IsHoldInteraction() const
{
	return bHoldToInteract;
}

void UInteractiveComponent::UpdateInteractionWidgetProgress(float CurrentHoldTime)
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetClass()->ImplementsInterface(UInterface_InteractionReceiver::StaticClass()))
		{
			IInterface_InteractionReceiver::Execute_UpdateInteractionProgress(InteractionWidget, CurrentHoldTime/HoldTime);
		}
	}
}

void UInteractiveComponent::Init(UPrimitiveComponent* Collision)
{
	InteractionCollision = Collision;
	SetInteraction(bActive);
	if (!InteractionWidgetComponent)
	{
		InteractionWidgetComponent = Cast<UWidgetComponent>(GetOwner()->AddComponentByClass(UWidgetComponent::StaticClass(), true, FTransform::Identity, false));
		InteractionWidgetComponent->AttachToComponent(InteractionCollision.Get(), FAttachmentTransformRules::KeepRelativeTransform);
		
		if (bOverrideDefaultWidget)
		{
			InteractionWidgetComponent->SetWidgetClass(InteractionWidgetClassOverride);
		}
		else
		{
			const auto* PlayerInteractionSettings = GetDefault<UPlayerInteractionSettings>();
			InteractionWidgetComponent->SetWidgetClass(PlayerInteractionSettings->GetDefaultInteractionWidgetClass());
		}
		
		InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InteractionWidgetComponent->AddLocalOffset(InteractionPointOffset);
	}
	InteractionWidget = InteractionWidgetComponent->GetUserWidgetObject();
	ensure(InteractionWidget);
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		if (bOverrideDefaultWidget)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: InteractionWidgetClassOverride not set"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DefaultInteractionWidgetClass not set in the Project Settings"));
		}
	}
}

UPrimitiveComponent* UInteractiveComponent::GetInteractionCollision() const
{
	return InteractionCollision.Get();
}