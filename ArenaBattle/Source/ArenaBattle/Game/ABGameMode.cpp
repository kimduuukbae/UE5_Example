// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
#include "Player/ABPlayerController.h"

AABGameMode::AABGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> defaultPawn = TEXT("/Script/ArenaBattle.ABCharacterPlayer");

	if (defaultPawn.Class)
		DefaultPawnClass = defaultPawn.Class;

	PlayerControllerClass = AABPlayerController::StaticClass();
}