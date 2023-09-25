#include "CharacterStat/ABCharacterStatComponent.h"


UABCharacterStatComponent::UABCharacterStatComponent()
{
	MaxHP = 200.0f;
}

void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHP(MaxHP);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHP(PrevHP - ActualDamage);

	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		//OnHPZero.Broadcast();
	}

	return ActualDamage;
}

void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp<float>(NewHP, 0.0f, MaxHP);

	//OnHPChanged.Broadcast(CurrentHP);
}



