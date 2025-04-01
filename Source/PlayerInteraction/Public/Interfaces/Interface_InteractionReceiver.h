// Copyright by LG7, 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_InteractionReceiver.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInterface_InteractionReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PLAYERINTERACTION_API IInterface_InteractionReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction Receiver Interface")
	void OnInteractionDetected(ACharacter* Character, bool bInRange, const FText& InteractionText);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction Receiver Interface")
	void UpdateInteractionText(const FText& InteractionText);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction Receiver Interface")
	void UpdateInteractionProgress(float Progress);
};
