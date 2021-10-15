// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTDGameMode.h"
#include "BTDCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABTDGameMode::ABTDGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_BaseCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
