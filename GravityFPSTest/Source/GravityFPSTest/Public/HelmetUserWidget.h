// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HelmetUserWidget.generated.h"

/**
 *
 */
UCLASS()
class GRAVITYFPSTEST_API UHelmetUserWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:

	UFUNCTION(BlueprintCallable)
	void PlayFadeIn();

	UFUNCTION(BlueprintCallable)
	void PlayFadeOut();

	enum class EHelmetState
	{
		Hidden,
		FadingIn,
		Visible,
		FadingOut
	};

protected:
	float pct;

	EHelmetState helmetState;

};