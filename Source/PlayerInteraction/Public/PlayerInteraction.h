// Copyright by LG7, 2024

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPlayerInteractionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
