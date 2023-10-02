#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "Engine/StreamableManager.h"
#include "ABCharacterNonePlayer.generated.h"

UCLASS(config=ArenaBattle)
class ARENABATTLE_API AABCharacterNonePlayer : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	AABCharacterNonePlayer();

protected:
	virtual void PostInitializeComponents() override;

protected:
	void SetDead() override;
	
	void NPCMeshLoadCompleted();

	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	TSharedPtr<FStreamableHandle> NPCMeshHandle;
};
