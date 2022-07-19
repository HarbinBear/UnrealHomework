// Copyright Epic Games, Inc. All Rights Reserved.

#include "CppProjectOnAndroidGameMode.h"
#include "CppProjectOnAndroidCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACppProjectOnAndroidGameMode::ACppProjectOnAndroidGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
