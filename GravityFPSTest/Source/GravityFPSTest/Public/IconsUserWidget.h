// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IconsUserWidget.generated.h"

/**
 *  TODO: create an array of Equipment Icons inside of the blueprints using each of the images, then make it so that each image appears
 * when you scroll the mouse wheel.
 */
class UImage;
UCLASS()
class GRAVITYFPSTEST_API UIconsUserWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icons")
	TMap<FName, UTexture2D*> EquipmentIcons;

	UPROPERTY(meta = (BindWidget))
	UImage* EquipmentImage;

	void SetDisplayLabel(FName name);

protected:
	float pct;
	UTexture2D* IconToDisplay;

//	DO NOT IGNORE ME. // will force the program to crash. Comment this out to run it. This is here so that you read the TODO line at the top and don't forget about it.
};
