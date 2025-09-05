// Fill out your copyright notice in the Description page of Project Settings.


#include "InvisibilityHUDUserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GravityFPSTest/GravityFPSTestCharacter.h"

bool UInvisibilityHUDUserWidget::Initialize()
{
    bool bResult = Super::Initialize();
    if (!bResult)
    {
        return false;
    }
    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("CountDownText"));
    if (Widget != nullptr)
    {
        Widget->SetText(FText::FromString("00:00"));
    }
    return true;
}

void UInvisibilityHUDUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    DisplayCountDown();
}

void UInvisibilityHUDUserWidget::DisplayCountDown()
{
    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("CountDownText"));
    AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* Pawn = pController->GetPawn();
    AGravityFPSTestCharacter* pPlayerCharacter = Cast<AGravityFPSTestCharacter>(Pawn);

    // GetInvisibilityCountDownDuration returns the countdown in seconds, but we want to display it to the user as a clock in minutes and seconds.
    // We add 1.0f to offset the rounding error.
    float CurrentTimeInSeconds = pPlayerCharacter->GetInvisibilityCountDownDuration() + 1.0f;
    int CurrentTimeInMinutes = 0;

    // Convert extra seconds into minutes.
    while (CurrentTimeInSeconds >= 60.0f)
    {
        CurrentTimeInSeconds -= 60.0f;
        CurrentTimeInMinutes += 1;
    }

    Widget->SetText(FText::FromString(FString::Printf(TEXT("%2d:%02d"), CurrentTimeInMinutes, (int)CurrentTimeInSeconds)));
}