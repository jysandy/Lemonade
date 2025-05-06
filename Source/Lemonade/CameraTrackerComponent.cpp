// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraTrackerComponent.h"
#include "Kismet/GameplayStatics.h"

UCameraTrackerComponent::UCameraTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bTickEvenWhenPaused = true;
}


void UCameraTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

    AActor* Camera = FindSideScrollingCamera();

    if (Camera != nullptr)
    {
        SideScrollingCamera = Camera;
    }

}


void UCameraTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();

    if (Owner != nullptr && SideScrollingCamera != nullptr)
    {
        FRotator CameraRotation = SideScrollingCamera->GetActorRotation();
        FRotator OwnerRotation = Owner->GetActorRotation();
        Owner->SetActorRotation(FRotator(OwnerRotation.Pitch,
            CameraRotation.Yaw,
            OwnerRotation.Roll));
    }
}

AActor* UCameraTrackerComponent::FindSideScrollingCamera()
{
    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(),
        TEXT("BP_SideScrollingCamera"),
        ActorsWithTag);

    for (AActor* Actor : ActorsWithTag)
    {
        if (Actor)
        {
            return Actor;
        }
    }

    return nullptr;
}

