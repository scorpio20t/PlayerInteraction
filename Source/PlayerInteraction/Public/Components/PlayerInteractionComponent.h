// Copyright by LG7, 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInteractionComponent.generated.h"


UENUM(BlueprintType)
enum class EInteractionState : uint8
{
	NO_OBJECT,
	OBJECT_DETECTED,
	OBJECT_IN_RANGE
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYERINTERACTION_API UPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInteractionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Channel used to detect interaction. Interaction collision volumes should BLOCK this channel
	UPROPERTY(EditAnywhere, Category = "Player Interaction")
	TEnumAsByte<ECollisionChannel> InteractionChannel = ECollisionChannel::ECC_Visibility;
	//When at this range, Interactive object is visible for the interaction system, but not interactable yet
	UPROPERTY(EditAnywhere, Category = "Player Interaction")
	float InteractionDetectionRange = 1000.f;
	//When at this range, Interactive object becomes interactable
	UPROPERTY(EditAnywhere, Category = "Player Interaction")
	float InteractionRange = 300.f;
	//In most cases we don't want to see interaction widget when camera is fully faded
	UPROPERTY(EditAnywhere, Category = "Player Interaction")
	bool bFullCameraFadeDisablesInteraction = true;

	//Handles on-press interaction
	UFUNCTION(BlueprintCallable, Category = "Player Interaction")
	void TryInteract();

	//Handles on-hold interaction end
	UFUNCTION(BlueprintCallable, Category = "Player Interaction")
	void StopHoldingInteract();

	UFUNCTION(BlueprintPure, Category = "Player Interaction")
	float GetCurrentInteractionHoldTime() const;

	UPROPERTY(BlueprintReadOnly, Category = "Player Interaction")
	TObjectPtr<class UInteractiveComponent> CurrentlyDetectedComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Player Interaction")
	EInteractionState InteractionState = EInteractionState::NO_OBJECT;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Player Interaction")
	bool bIsHoldInteractInProgress = false;

private:
	float CurrentHoldTime = 0.f;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> Owner = nullptr;
	UPROPERTY()
	TWeakObjectPtr<class ACharacter> OwningCharacter = nullptr;
	UPROPERTY()
	TWeakObjectPtr<class UCameraComponent> CameraComponent = nullptr;
	UPROPERTY()
	TWeakObjectPtr<class APlayerCameraManager> PlayerCameraManager = nullptr;

	void CheckInteractionTrace();
	float GetDistanceFromDetectedComponent() const;
	void CheckDistanceFromDetectedComponent();
	void ClearDetectedComponent();
	void TryUpdateHoldInteraction(float DeltaTime);

	class UInteractiveComponent* FindInteractiveComponentForCollision(AActor* HitActor, UPrimitiveComponent* Collision);
};