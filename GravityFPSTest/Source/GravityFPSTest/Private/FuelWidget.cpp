// Fill out your copyright notice in the Description page of Project Settings.


#include "FuelWidget.h"

bool UFuelWidget::Initialize()
{
	bool bResult = Super::Initialize();

	if (!bResult)
	{
		return false;
	}
	return true;
}

void UFuelWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

}

void UFuelWidget::SetBarPercent(float Percent)
{
    if (FuelProgressBar)
    {
        FuelProgressBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
    }
}