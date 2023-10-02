#include "CharacterStat/ABCharacterStatComponent.h"
#include "GameData/ABGameSingleton.h"

UABCharacterStatComponent::UABCharacterStatComponent()
{
	CurrentLevel = 1;
}

void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetLevelStat(CurrentLevel);
	SetHP(BaseStat.MaxHP);
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevel)
{
	CurrentLevel = FMath::Clamp(InNewLevel, 1, UABGameSingleton::Get().CharacterMaxLevel);
	BaseStat = UABGameSingleton::Get().GetCharacterStat(CurrentLevel);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHP(PrevHP - ActualDamage);

	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		OnHPZero.Broadcast();
	}

	return ActualDamage;
}

void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp<float>(NewHP, 0.0f, BaseStat.MaxHP);

	OnHPChanged.Broadcast(CurrentHP);
}



