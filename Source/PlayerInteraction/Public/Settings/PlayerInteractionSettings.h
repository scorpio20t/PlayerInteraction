// Copyright by LG7, 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInteractionSettings.generated.h"

/**
 * 
 */
UCLASS(config = UI, defaultconfig, meta = (DisplayName = "Player Interaction"))
class PLAYERINTERACTION_API UPlayerInteractionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Player Interaction")
	TSubclassOf<UUserWidget> GetDefaultInteractionWidgetClass() const;

protected:

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Player Interaction")
	TSubclassOf<UUserWidget> DefaultInteractionWidgetClass;
	
};
