// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTDGameMode.h"
#include "BTDCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABTDGameMode::ABTDGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Charcters/BP_NetCharacter.BP_NetCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
