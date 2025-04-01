// Copyright by LG7, 2024


#include "Settings/PlayerInteractionSettings.h"

TSubclassOf<UUserWidget> UPlayerInteractionSettings::GetDefaultInteractionWidgetClass() const
{
	return DefaultInteractionWidgetClass;
}