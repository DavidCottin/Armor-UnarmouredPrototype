// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Public/UTargetableInterface.h"
#include "GravityFPSTestCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UNiagaraSystem;
class UAudioComponent;
class USoundBase;
class UBiopadComponent;
class UCapsuleComponent;
class UFlyingTimerComponent;
class UTP_WeaponComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, FName, NewEquipment);
/**
 *  Player Character class
 *  Handles common functionality, such as input handling, weapon switching, weapon firing, and camera management.
 *  Specific weapon functionality is handled in sub classes.
 */

UCLASS(config=Game)
class AGravityFPSTestCharacter : public ACharacter, public ITargetableInterface
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	
	/// <summary>
	/// This enumerator keeps track of which weapon the player has equipped while wearing their armour. They can swap their weapon at any time by
	/// scrolling with the mouse wheel.
	/// </summary>
	enum class EArmourWeaponState
	{
		Laser         UMETA(DisplayName = "Laser"),
		EmergencyCube UMETA(DisplayName = "EmergencyCube"),
		Missile		  UMETA(DisplayName = "Missile"),
		TankRifle	  UMETA(DisplayName = "Tank Rifle"),
		Invisibility  UMETA(DisplayName = "Invisibility"),
		Biopad        UMETA(DisplayName = "Biopad"),
		MAX           UMETA(Hidden),
		
	};
	EArmourWeaponState ArmouredWeapon;

	enum class EHumanWeaponState
	{
		Hands         UMETA(DisplayName = "Hands"),
		Gun         UMETA(DisplayName = "Gun"),
		EmergencyCube UMETA(DisplayName = "EmergencyCube"),
		MAX           UMETA(Hidden),

	};
	EHumanWeaponState HumanWeapon;

	
public:
	AGravityFPSTestCharacter();

	// Getters and Setters
	bool IsThrusting() { return bIsThrusting; };
	UFlyingTimerComponent* GetFuelComponent() { return FuelComponent; };
	bool IsWearingArmour() { return bIsWearingArmour; };
	bool IsInvisible() { return bIsInvisible; };
	FVector GetSavedLocation() { return SavedLocation; };
	UBiopadComponent* GetBiopadComponent() { return BiopadComponent; };
	TArray<AActor*> GetActorsInSphereFromCamera(float Radius, float TraceDist, float ConeAngle, FCollisionQueryParams Params);
	TArray<AActor*> GetActorsInConeFromCamera(float Radius, float TraceDist, float ConeAngle);
	float GetInvisibilityCountDownDuration() { return InvisibilityTimer; };

	void KeyUp() { bIsKeyDown = false; };
	void ResetJumps() { JumpMaxCount = 1; };
	void JumpKeyPressed(bool b) { bHoldingJumpButton = b; };
	void SetFlightAbility(bool b) { bCanFly = b; };

	void PlayLaserSound();
	void StopLaserSound();
	void SpacebarReleased();
	void ResetPhysics();

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentChanged OnEquipmentChanged;
	UFUNCTION()
	void SwitchEquipment(FName EquipmentName) { OnEquipmentChanged.Broadcast(EquipmentName); };

protected:
	virtual void BeginPlay();	

	void HideSocketComponents();
	void ShowSelectedSocketComponent();
	FName ConvertWeaponToString(int i);

	// Armour data members
	bool bIsWearingArmour;
	bool bIsKeyDown;
	bool bIsInvisible;
	bool bCanFly;
	float InvisibilityTimer;
	UFlyingTimerComponent* FuelComponent;

	// Human functions
	void WallDetect();
	UTP_WeaponComponent* GetAnyWeaponComponent(UWorld* World);

	// Human data members
	float MomentumSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* PlayerWallDetector;
	bool bOnWall;
	bool bHoldingJumpButton;
	FVector WallNormal;
	TArray<FHitResult> ValidWallsInRange;

	UTP_WeaponComponent* GunReference;
	ACubeProjectile* CubeReference;

	// Flying functions
	bool IsTouchingAnySurface();

	// Flying data members
	bool bIsThrusting;
	FVector CurrentThrustInput;
	FVector FlyingVelocity;
	FRotator RotationWhenThrustersDisabled;

	// Shooting data members
	float TimeSinceLastShot;

	// Teleportation data members
	FVector SavedLocation;

	// Tank rifle data members
	float NukeCharge;

	// Biopad data members
	UBiopadComponent* BiopadComponent;
public:
	/// <summary>
	/// If properly setup, this niagara system is intended to be used to create special in game effects for when the laser is used in game
	/// However, due to my lack of knowledge in niagara systems, it is currently unused in the code.
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* BeamEffect;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> LaserToSpawn;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> CubeToSpawn;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> MissileToSpawn;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> NukeToSpawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* LaserAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* LaserSoundBase;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHoldingObject;


	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHoldingObject(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHoldObjectState();

public:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for crouching input */
	void StartCrouching(const FInputActionValue& Value);
	void StopCrouching(const FInputActionValue& Value);

	/** Called for flying input */
	void StartThrusters(const FInputActionValue& Value);

	/** Called for flying input */
	void EndThrusters(const FInputActionValue& Value);

	/** Called for laser input */
	void ShootLasers(const FInputActionValue& Value);

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

	/** Called for Invisibility input */
	void ToggleInvisibility();

	/** Called for Biopad input*/
	void ScanObject();
	void RemoveLastSelectedObject();

	/** Called for interaction input*/
	void DetectDoor();

	/** Called for Swap Armour input*/
	void SwapArmour();
	
protected:
	// APawn interface

	virtual void Tick(float DeltaTime) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};