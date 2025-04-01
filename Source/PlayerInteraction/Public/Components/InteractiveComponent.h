// Copyright by LG7, 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionDelegate, ACharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDetectedDelegate, ACharacter*, Character, bool, bInInteractionRange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLostDelegate, ACharacter*, Character);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLAYERINTERACTION_API UInteractiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractiveComponent();

	//Is object currently interactable?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Interaction")
	bool bActive = true;
	//Text displayed on the interaction widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Interaction")
	FText InteractionText = FText::FromString("Interact");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Interaction")
	bool bHoldToInteract = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bHoldToInteract"), Category = "Player Interaction")
	float HoldTime = 1.0f;

	//Interaction Point offset, relative to Interaction Collision location
	UPROPERTY(EditAnywhere, Category = "Player Interaction")
	FVector InteractionPointOffset;

	//Set to TRUE if you want to display a custom interaction widget (not the one set in the Project Settings)
	UPROPERTY(EditAnywhere, Category = "Player Interaction")
	bool bOverrideDefaultWidget = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bOverrideDefaultWidget"), Category = "Player Interaction")
	TSubclassOf<class UUserWidget> InteractionWidgetClassOverride;

	UPROPERTY(BlueprintReadWrite, Category = "Player Interaction")
	TObjectPtr<class UWidgetComponent> InteractionWidgetComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Player Interaction")
	TObjectPtr<class UUserWidget> InteractionWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Player Interaction")
	TWeakObjectPtr<class ACharacter> LastInteractingCharacter = nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionDelegate OnInteraction;

	UPROPERTY(BlueprintAssignable)
	FOnDetectedDelegate OnDetectedNotify;

	UPROPERTY(BlueprintAssignable)
	FOnLostDelegate OnLostNotify;

	UFUNCTION()
	void OnDetected(ACharacter* Character, bool InRange);

	UFUNCTION()
	void OnInteract(ACharacter* Character);

	UFUNCTION()
	void OnLost(ACharacter* Character);

	//Sets whether the object is interactive or not
	UFUNCTION(BlueprintCallable, Category = "Player Interaction")
	void SetInteraction(bool Active);
	//Sets the interaction text
	UFUNCTION(BlueprintCallable, Category = "Player Interaction")
	void SetInteractionText(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = "Player Interaction")
	void SetHoldToInteract(bool bShouldHoldToInteract);

	UFUNCTION(BlueprintPure, Category = "Player Interaction")
	bool IsHoldInteraction() const;

	UFUNCTION(BlueprintCallable, Category = "Player Interaction")
	void UpdateInteractionWidgetProgress(float CurrentHoldTime);
	//Initializes the Interactive Component, by providing the Interaction Collision and initializing the interaction widget
	UFUNCTION(BlueprintCallable, Category = "Player Interaction")
	void Init(class UPrimitiveComponent* Collision);

	UFUNCTION(BlueprintPure, Category = "Player Interaction")
	class UPrimitiveComponent* GetInteractionCollision() const;

private:
	UPROPERTY()
	TWeakObjectPtr<class UPrimitiveComponent> InteractionCollision = nullptr;
};