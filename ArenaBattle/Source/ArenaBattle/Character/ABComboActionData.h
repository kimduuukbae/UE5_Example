#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABComboActionData.generated.h"

UCLASS()
class ARENABATTLE_API UABComboActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UABComboActionData();

	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix;			// Montage Section Name

	UPROPERTY(EditAnywhere, Category = Name)
	uint8 MaxComboCount;						// �� ��� �ִϸ��̼��� �ִ���

	UPROPERTY(EditAnywhere, Category = Name)
	float FrameRate;							// �Էµ� �������� ����

	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;			// �Էµ� �����ӵ��� �ִ����� üũ 
};

