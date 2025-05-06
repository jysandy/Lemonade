// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerDoor.h"
#include "PlayableCharacter.h"


ATriggerDoor::ATriggerDoor()
{
    SetActorRelativeScale3D({ 0.5f, 2.f, 2.f });
}

void ATriggerDoor::NotifyActorBeginOverlap(AActor* OtherActor)
{                                          
    Super::NotifyActorBeginOverlap(OtherActor);

    APlayableCharacter* Player = Cast<APlayableCharacter>(OtherActor);
    if (Player != nullptr)
    {
        auto Location = GetActorLocation();
        auto PlayerLocation = Player->GetActorLocation();

        EntryDirection 
            = (PlayerLocation - Location).ProjectOnTo(GetActorForwardVector());
    }
}

void ATriggerDoor::NotifyActorEndOverlap(AActor* OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);

    APlayableCharacter* Player = Cast<APlayableCharacter>(OtherActor);
    if (Player != nullptr)
    {
        auto Location = GetActorLocation();
        auto PlayerLocation = Player->GetActorLocation();
        auto ExitDirection 
            = (PlayerLocation - Location).ProjectOnTo(GetActorForwardVector());

        if (!EntryDirection.IsZero() 
            && EntryDirection.Dot(ExitDirection) < 0)
        {
            PlayerWalkedThrough(ExitDirection);
            OnPlayerWalkedThrough.Broadcast(ExitDirection);
        }

        EntryDirection = FVector::ZeroVector;
    }
}
