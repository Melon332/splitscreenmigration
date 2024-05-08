// Copyright Epic Games, Inc. All Rights Reserved.

#include "SplitScreenCPPGameMode.h"
#include "SplitScreenCPPCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASplitScreenCPPGameMode::ASplitScreenCPPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
