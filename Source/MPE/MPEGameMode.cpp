// Copyright Epic Games, Inc. All Rights Reserved.

#include "MPEGameMode.h"
#include "MPECharacter.h"
#include "UObject/ConstructorHelpers.h"

AMPEGameMode::AMPEGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
