// Copyright Epic Games, Inc. All Rights Reserved.

#include "LemonadeGameMode.h"
#include "LemonadeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALemonadeGameMode::ALemonadeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Lemonade/Blueprints/Characters/BP_RaccoonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
