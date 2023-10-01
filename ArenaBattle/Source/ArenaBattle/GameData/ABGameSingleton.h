#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterStat.h"
#include "ABGameSingleton.generated.h"

#if WITH_EDITOR
DECLARE_LOG_CATEGORY_EXTERN(LogABGAmeSingleton, Error, All);
#endif

UCLASS()
class ARENABATTLE_API UABGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UABGameSingleton();
	static UABGameSingleton& Get();

public:
	FORCEINLINE FABCharacterStat GetCharacterStat(int32 InLevel) const
	{
		return CharacterStatTable.IsValidIndex(InLevel) ? CharacterStatTable[InLevel] : FABCharacterStat{};
	}

	UPROPERTY()
	int32 CharacterMaxLevel;

private:
	TArray<FABCharacterStat> CharacterStatTable;
};
