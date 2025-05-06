// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SideScrollingCamera.h"
#include "LemonadeGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLemonadeGameInstance, Log, All);

/**
 * 
 */
UCLASS()
class LEMONADE_API ULemonadeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public: 
	UFUNCTION(BlueprintCallable)
	void SaveGame(FVector PlayerLocation);

	UFUNCTION(BlueprintCallable)
	ULemonadeSaveGame* LoadGame();

	UFUNCTION(BlueprintCallable)
	void ClearSave();

	UFUNCTION(BlueprintCallable)
	void RespawnPlayer(ULemonadeSaveGame* SaveInstance, float Delay = 0.5f);

	UFUNCTION()
	virtual void PlayerDied();

	// Level state

	FString GetLevelState(const FString& LevelName, const FString& Key);

	void SetLevelState(const FString& LevelName,
		const FString& Key, const FString& Value);

	// Get a key from the level state. 
	// Returns empty if the key doesn't exist.
	UFUNCTION(BlueprintCallable)
	FString GetCurrentLevelState(const FString& Key);

	// Set an entry in the level state. 
	// These entries will be saved when the player 
	// saves the game.
	UFUNCTION(BlueprintCallable)
	void SetCurrentLevelState(const FString& Key,
		const FString& Value);

	UFUNCTION(BlueprintCallable)
	ALemonadeLevelScriptActor* GetLevelScriptActor();

private:
	using LevelData = TMap<FString, FString>;

	void PassStateToLevel();

	FString CurrentLevelName() const;
	ASideScrollingCamera* FindSideScrollingCamera();

	TMap<FString, LevelData> LevelState;

	FTimerHandle RestartTimer;
};
