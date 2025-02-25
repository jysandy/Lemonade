// Fill out your copyright notice in the Description page of Project Settings.


#include "SideScrollingCamera.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ASideScrollingCamera::ASideScrollingCamera()
{
    PrimaryActorTick.bCanEverTick = true;
    CameraZOffset = 400.f;
}

void ASideScrollingCamera::BeginPlay()
{
    Super::BeginPlay();

    FVector CurrentLocation = GetActorLocation();
    CameraX = CurrentLocation.X;
}

void ASideScrollingCamera::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (Player != nullptr)
    {
        FVector PlayerLocation = Player->GetActorLocation();

        RootComponent->SetWorldLocation({
               CameraX,
               PlayerLocation.Y,
               PlayerLocation.Z + CameraZOffset
            });
    }
}