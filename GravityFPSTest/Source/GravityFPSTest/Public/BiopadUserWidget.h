// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BiopadUserWidget.generated.h"

class AGravityFPSTestCharacter;
/**
 * This class is responsible for displaying text on the screen that shows the whereabouts of the player's
 * position in reference to logged objects. The logging logic happens in the BiopadComponent class.
 */
UCLASS()
class GRAVITYFPSTEST_API UBiopadUserWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void ToggleDistance() { DistanceInsteadOfCoordinates = !DistanceInsteadOfCoordinates; };

protected:
	FString FormatSignedInt(int32 Value);

	AGravityFPSTestCharacter* PlayerCharacter;

	bool DistanceInsteadOfCoordinates;

	/** Reference to the Text Block in the UI */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BiopadScreenText;
};
