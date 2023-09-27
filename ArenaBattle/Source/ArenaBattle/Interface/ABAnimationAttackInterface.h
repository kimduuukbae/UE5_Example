#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ABAnimationAttackInterface.generated.h"

UINTERFACE(MinimalAPI)
class UABAnimationAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class ARENABATTLE_API IABAnimationAttackInterface
{
	GENERATED_BODY()

public:
	virtual void AttackHitCheck() = 0;
};
