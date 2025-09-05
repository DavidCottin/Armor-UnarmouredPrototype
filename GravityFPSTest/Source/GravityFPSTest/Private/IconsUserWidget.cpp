// Fill out your copyright notice in the Description page of Project Settings.


#include "IconsUserWidget.h"
#include "Components/Image.h"
#include "Constants.h"


bool UIconsUserWidget::Initialize()
{
    bool bResult = Super::Initialize();
    pct = 0.0f;
    SetRenderOpacity(pct);
    if (!bResult)
    {
        return false;
    }
    return true;
}

void UIconsUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (IconToDisplay && pct > 0.0f)
    {
        pct -= InDeltaTime;
        float LerpedValue = FMath::Lerp(0.0f, 1.0f, pct);
        SetRenderOpacity(LerpedValue);
    }
}

void UIconsUserWidget::SetDisplayLabel(FName name)
{
    UTexture2D* FoundIcon = EquipmentIcons.FindRef(name);
    if (!FoundIcon)
    {
        UE_LOG(LogTemp, Warning, TEXT("No icon found for %s"), *name.ToString());
        return;
    }

    IconToDisplay = FoundIcon;
    pct = 1.0f;

    // Actually update the image brush
    if (EquipmentImage && IconToDisplay)
    { 
        EquipmentImage->SetBrushFromTexture(IconToDisplay, true);
    }
}