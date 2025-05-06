// Fill out your copyright notice in the Description page of Project Settings.


#include "Lemonade2DCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "SideScrollingCamera.h"


ASideScrollingCamera* ALemonade2DCharacter::FindSideScrollingCamera()
{
    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(),
        TEXT("BP_SideScrollingCamera"),
        ActorsWithTag);

    for (AActor* Actor : ActorsWithTag)
    {
        if (Actor)
        {
            return Cast<ASideScrollingCamera>(Actor);
        }
    }

    return nullptr;
}

void ALemonade2DCharacter::BeginPlay()
{
    Super::BeginPlay();

    SideScrollingCamera = FindSideScrollingCamera();
}

FVector ALemonade2DCharacter::GetCameraRotatedVelocity() const
{
    auto velocity = this->GetVelocity();
    if (!SideScrollingCamera->IsLookingDownPositiveX())
    {
        auto Rotator = FQuat(FVector::UnitZ(),
            FMath::DegreesToRadians(-90.0));
        velocity = Rotator.RotateVector(velocity);
    }

    return velocity;
}

FVector ALemonade2DCharacter::GetCameraRotatedVector(FVector In) const
{
    FVector Out = In;
    if (!SideScrollingCamera->IsLookingDownPositiveX())
    {
        auto Rotator = FQuat(FVector::UnitZ(),
            FMath::DegreesToRadians(-90.0));
        Out = Rotator.RotateVector(In);
    }

    return Out;
}

