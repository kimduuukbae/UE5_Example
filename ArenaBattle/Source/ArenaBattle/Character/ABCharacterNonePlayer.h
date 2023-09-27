#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "ABCharacterNonePlayer.generated.h"

UCLASS()
class ARENABATTLE_API AABCharacterNonePlayer : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	AABCharacterNonePlayer();

protected:
	void SetDead() override;
	
};
