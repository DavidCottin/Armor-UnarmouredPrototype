// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravityFPSTestGameMode.h"
#include "GravityFPSTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGravityFPSTestGameMode::AGravityFPSTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
