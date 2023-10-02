#include "GameData/ABGameSingleton.h"

#if WITH_EDITOR
DEFINE_LOG_CATEGORY(LogABGameSingleton)
#endif

UABGameSingleton::UABGameSingleton()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT(""));

	if (DataTableRef.Object)
	{
		const UDataTable* DataTable = DataTableRef.Object;

		TArray<uint8*> ValueArray;
		DataTable->GetRowMap().GenerateValueArray(ValueArray);
		Algo::Transform(ValueArray, CharacterStatTable,
			[](uint8* Value)
			{
				return *reinterpret_cast<FABCharacterStat*>(Value);
			}
		);
	}

	CharacterMaxLevel = CharacterStatTable.Num();
}

UABGameSingleton& UABGameSingleton::Get()
{
	UABGameSingleton* Singleton = CastChecked<UABGameSingleton>(GEngine->GameSingleton);
	if (Singleton)
		return *Singleton;

#if WITH_EDITOR
	UE_LOG(LogABGameSingleton, Error, TEXT("Invalid Game Singleton"));
#endif

	return *NewObject<UABGameSingleton>();
}
