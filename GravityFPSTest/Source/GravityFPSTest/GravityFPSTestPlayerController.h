// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GravityFPSTestPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UHelmetUserWidget;
class UInvisibilityHUDUserWidget;
class URadarUserWidget;
class URadarMap;
class UBiopadUserWidget;
class UFuelWidget;
class UIconsUserWidget;
class AGravityFPSTestCharacter;
struct FInputActionValue;

/**
 *
 */
UCLASS()
class GRAVITYFPSTEST_API AGravityFPSTestPlayerController : public APlayerController
{
	GENERATED_BODY()

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Fly Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FlyAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Laser Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LaserAction;

	/** ScrollWheel Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ScrollWheelAction;

	/** Left Click Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DefaultLeftClickAction;

	/** Right Click Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DefaultRightClickAction;

	/** Middle Click Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DefaultMiddleClickAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/** Tank Rifle Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TankRifleAction;

	/** Swap Mode Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	
protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void KeyUp();

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for jumping input */
	void Jump(const FInputActionValue& Value);
	void StopJumping(const FInputActionValue& Value);

	/** Called for crouching input */
	void StartCrouching(const FInputActionValue& Value);
	void StopCrouching(const FInputActionValue& Value);

	/** Called for flying input */
	void StartThrusters(const FInputActionValue& Value);

	/** Called for flying input */
	void EndThrusters(const FInputActionValue& Value);

	/** Called for laser input */
	void ShootLasers(const FInputActionValue& Value);
	void PlayLaserSound();
	void StopLaserSound();

	/** Called for scrolling input */
	void CycleAbilityByMouse(const FInputActionValue& Value);

	/** Called for Default Left Click input */
	void DropCube(const FInputActionValue& Value);

	/** Called for Default Right Click input */
	void SaveLocation();

	/** Called for Default Left Click input */
	void FireMissile();

	/** Called for Tank Rifle input */
	void ChargeNuke();
	void FireNuke();

	/** Called for Invisibility input*/
	void ToggleInvisibility();

	/** Called for Biopad input*/
	void ScanObject();
	void RemoveLastSelectedObject();
	void SwitchBiopadDisplay();

	/** Called for interaction input*/
	void DetectDoor();

	/** Called for Swap input*/
	void SwapArmour();


public:
	void ShowInvisibilityWidget();
	void HideInvisibilityWidget();
	void ShowFuelWidget();
	void HideFuelWidget();
	void ShowRadar();
	void HideRadar();
	UFuelWidget* GetFuelWidget() { return FuelWidget; };

protected:
	virtual void AcknowledgePossession(APawn* InPawn) override;

	// Begin Actor interface
protected:

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	UFUNCTION()
	void HandleEquipmentChanged(FName name);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UHelmetUserWidget> HelmetWidgetClass;
	UHelmetUserWidget* HelmetWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UInvisibilityHUDUserWidget> InvisibilityWidgetClass;
	UInvisibilityHUDUserWidget* InvisibilityWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<URadarUserWidget> RadarWidgetClass;
	URadarUserWidget* RadarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UBiopadUserWidget> BiopadWidgetClass;
	UBiopadUserWidget* BiopadWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UFuelWidget> FuelWidgetClass;
	UFuelWidget* FuelWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UIconsUserWidget> IconsWidgetClass;
	UIconsUserWidget* IconWidget;

	// pointer to the controlled pawn
	AGravityFPSTestCharacter* MyCharacter;

	// End Actor interface
};
