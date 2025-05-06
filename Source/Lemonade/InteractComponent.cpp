// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractComponent.h"

void UInteractComponent::TriggerInteractEvent()
{
    OnInteract.Broadcast();
}

void UInteractComponent::TriggerEnterEvent()
{
    OnPlayerEnteredInteractRange.Broadcast();
}

void UInteractComponent::TriggerExitEvent()
{
    OnPlayerExitedInteractRange.Broadcast();
}
