// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TriggerDoor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerWalkedThrough, FVector, ExitDirection);

/**
 * 
 */
UCLASS()
class LEMONADE_API ATriggerDoor : public ATriggerBox
{
	GENERATED_BODY()
	

public:
	ATriggerDoor();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnPlayerWalkedThrough OnPlayerWalkedThrough;


protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Events")
	void PlayerWalkedThrough(FVector ExitDirection);

private:
	FVector EntryDirection = FVector::ZeroVector;
};
