#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ABCharacterWidgetInterface.generated.h"

UINTERFACE(MinimalAPI)
class UABCharacterWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class ARENABATTLE_API IABCharacterWidgetInterface
{
	GENERATED_BODY()

public:

	virtual void SetupCharacterWidget(class UUserWidget* InUserWidget) = 0;
};
