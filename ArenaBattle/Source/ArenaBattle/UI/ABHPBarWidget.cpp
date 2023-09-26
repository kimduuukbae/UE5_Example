// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHPBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/ABCharacterWidgetInterface.h"

UABHPBarWidget::UABHPBarWidget(const FObjectInitializer& ObjectInitializer) : Super::UUserWidget(ObjectInitializer)
{
	MaxHP = -1.0f;
}

void UABHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));

	ensure(HPProgressBar);

	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(GetOwner());

	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UABHPBarWidget::UpdateHPBar(float NewCurrentHP)
{
	ensure(MaxHP > 0.0f);

	if (!HPProgressBar) return;

	HPProgressBar->SetPercent(NewCurrentHP / MaxHP);
}
