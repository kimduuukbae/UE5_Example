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
	uint8 MaxComboCount;						// 총 몇개의 애니메이션이 있는지

	UPROPERTY(EditAnywhere, Category = Name)
	float FrameRate;							// 입력된 프레임의 기준

	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;			// 입력된 프레임들이 있는지를 체크 
};

