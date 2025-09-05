// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "RadarMap.generated.h"

/**
 * 
 */
class UBlipUserWidget;
UCLASS()
class GRAVITYFPSTEST_API URadarMap : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CP_Blips;

protected:
	void EventUpdateDetection();
	void FakeNativeTick();
	
	TArray<AActor*> EnemyActors;
	float DetectionRange;
	FVector2D WidgetRadiusInPixels;
	float WidgetScaleFactor;
	FTimerHandle OneSecondTimerHandle;
	FTimerHandle NativeTickHandle;
	FVector2D RadarSize;

	bool bInitialized;
	bool bMustUpdateRadarSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UBlipUserWidget> BlipWidgetClass;
//	UBlipUserWidget* BlipWidget;
};
