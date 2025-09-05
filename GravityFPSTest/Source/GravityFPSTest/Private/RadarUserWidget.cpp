// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarUserWidget.h"

bool URadarUserWidget::Initialize()
{
    bool bResult = Super::Initialize();
    if (!bResult)
    {
        return false;
    }
    return true;
}

void URadarUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

}
