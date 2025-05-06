// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraTrackerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEMONADE_API UCameraTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCameraTrackerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:

	UPROPERTY()
	AActor* SideScrollingCamera = nullptr;

	AActor* FindSideScrollingCamera();
};
