// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvisibilityHUDUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITYFPSTEST_API UInvisibilityHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void DisplayCountDown();

protected:
	/** Reference to the Text Block in the UI */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountDownText;
};
