// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "LemonadeLevelScriptActor.generated.h"

class ULemonadeGameInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveLoadedSignature);

/**
 * Base class for all Lemonade level blueprints.
 */
UCLASS()
class LEMONADE_API ALemonadeLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public: 
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnSaveLoaded();

	// Get a key from the level state. 
	// Returns empty if the key doesn't exist.
	UFUNCTION(BlueprintCallable)
	FString GetLevelState(const FString& Key);

	// Set an entry in the level state. 
	// These entries will be saved when the player 
	// saves the game.
	UFUNCTION(BlueprintCallable)
	void SetLevelState(const FString& Key, const FString& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> DefaultLevelState;

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Events")
	FOnSaveLoadedSignature OnSaveLoadedEvent;

	void TriggerOnSaveLoaded();

private:
	FString GetLevelName() const;
	ULemonadeGameInstance* GetGameInstance() const;

};
