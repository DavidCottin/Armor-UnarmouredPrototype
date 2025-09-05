// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "FuelWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITYFPSTEST_API UFuelWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	/** Sets the bar fill percentage (0.0 to 1.0) */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetBarPercent(float Percent);

protected:
	/** Bound to the ProgressBar in the UMG Designer */
	UPROPERTY(meta = (BindWidget))
	UProgressBar* FuelProgressBar;
	
};
