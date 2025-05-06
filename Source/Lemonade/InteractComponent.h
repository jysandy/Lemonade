// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "InteractComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEnteredInteractRange);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerExitedInteractRange);  


/**
 * Box component that can be interacted with. 
 * Interaction occurs when the player is inside the box and 
 * presses Interact.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LEMONADE_API UInteractComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Interact")
	FOnInteract OnInteract;

	UPROPERTY(BlueprintAssignable, Category="Interact")
	FOnInteract OnPlayerEnteredInteractRange;

	UPROPERTY(BlueprintAssignable, Category="Interact")
	FOnInteract OnPlayerExitedInteractRange;

	void TriggerInteractEvent();
	void TriggerEnterEvent();
	void TriggerExitEvent();

};
