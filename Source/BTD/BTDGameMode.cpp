// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTDGameMode.h"
#include "BTDCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABTDGameMode::ABTDGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
