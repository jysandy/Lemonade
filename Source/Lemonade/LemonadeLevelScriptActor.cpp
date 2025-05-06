// Fill out your copyright notice in the Description page of Project Settings.


#include "LemonadeLevelScriptActor.h"
#include "LemonadeGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ALemonadeLevelScriptActor::TriggerOnSaveLoaded()
{
    OnSaveLoaded();
    OnSaveLoadedEvent.Broadcast();
}

FString ALemonadeLevelScriptActor::GetLevelState(const FString& Key)
{
    return GetGameInstance()->GetLevelState(GetLevelName(), Key);
}

void ALemonadeLevelScriptActor::SetLevelState(const FString& Key,
    const FString& Value)
{
    GetGameInstance()->SetLevelState(GetLevelName(),
        Key,
        Value);
}

ULemonadeGameInstance* ALemonadeLevelScriptActor::GetGameInstance() const
{
    return Cast<ULemonadeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

FString ALemonadeLevelScriptActor::GetLevelName() const
{
    return GetLevel()->GetOutermost()->GetName();
}

