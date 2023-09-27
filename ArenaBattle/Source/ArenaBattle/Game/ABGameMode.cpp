#include "Game/ABGameMode.h"
#include "Player/ABPlayerController.h"

AABGameMode::AABGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawn = TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/BP_AB_Character.BP_AB_Character_C'");

	if (DefaultPawn.Class)
		DefaultPawnClass = DefaultPawn.Class;

	PlayerControllerClass = AABPlayerController::StaticClass();
}