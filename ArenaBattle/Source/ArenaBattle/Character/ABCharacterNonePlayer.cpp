#include "Character/ABCharacterNonePlayer.h"
#include "Engine/AssetManager.h"

AABCharacterNonePlayer::AABCharacterNonePlayer()
{
	GetMesh()->SetHiddenInGame(false);
}

void AABCharacterNonePlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	int32 RandIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);
	NPCMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCMeshes[RandIndex], FStreamableDelegate::CreateUObject(this, &AABCharacterNonePlayer::NPCMeshLoadCompleted));
}

void AABCharacterNonePlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]() 
		{
			Destroy();
		})
	, DeadEventDelayTime, false);
}

void AABCharacterNonePlayer::NPCMeshLoadCompleted()
{
	if (NPCMeshHandle.IsValid())
	{
		USkeletalMesh* NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());
		if (NPCMesh)
		{
			GetMesh()->SetSkeletalMesh(NPCMesh);
			GetMesh()->SetHiddenInGame(false);
		}
	}

	NPCMeshHandle->ReleaseHandle();
}
