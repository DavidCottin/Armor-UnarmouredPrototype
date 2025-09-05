// Copyright Epic Games, Inc. All Rights Reserved.



#include "GravityFPSTestCharacter.h"
#include "GravityFPSTestProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Constants.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LaserBeamProjectile.h"
#include "CubeProjectile.h"
#include "MissileProjectile.h"
#include "TankRifleProjectile.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "BiopadComponent.h"
#include "FlyingTimerComponent.h"
#include "TP_WeaponComponent.h"
#include "Components/BoxComponent.h"
#include "DoorInterface.h"
#include "MissileManager.h"
#include "GravityFPSTest/GravityFPSTestPlayerController.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGravityFPSTestCharacter

/**
 *  The Player has two main states, Armoured and Unarmoured. This is kept track of with the boolean data member - bIsWearingArmour.
 *  The majority of the player's logic allows them to use different weapons and abilities depending on if they are or are not wearing armour.
 *
 *  Abilities that are part of the armour set are included in the EArmourWeaponState enumerated class, and the active weapon is kept track of
 *  using the ArmouredWeapon data member. As of this version, an Unarmoured enumerated class and Unarmoued data member to keep track of the
 *  current weapon equipped do not exist, however there are plans to include them in the future.

    This class has been designed as effectively as possible to make the transition to include Unarmoued gameplay as seamless as possible.
    In our game, the player starts by wearing Armour, and (although not coded yet), will be allowed to switch the armour on or off with the press of a button.

    Any abilities that require Armour to be equipped or unequipped will make reference to bIsWearingArmour. Many of the weapons are all mapped to MB1,
    and so they are programmed to only activate if the weapon equipped is the correct type.

    If they happen to be the incorrect type, then the function is still
 *  called but simply will not do anything. (It will attempt to enter the conditional statement check and fail)
 *
 * For more information regarding input and widgets, refer to the player controller class - GravityFPSTestPlayerController
 */

AGravityFPSTestCharacter::AGravityFPSTestCharacter() : bIsThrusting(false), bIsWearingArmour(false), CurrentThrustInput(FVector::ZeroVector),
FlyingVelocity(FVector::ZeroVector), bIsKeyDown(false), RotationWhenThrustersDisabled(FRotator::ZeroRotator), TimeSinceLastShot(0.0f),
ArmouredWeapon(EArmourWeaponState::Laser), NukeCharge(0.0f), InvisibilityTimer(Constants::c_InvisibilityDuration), bIsInvisible(false),
HumanWeapon(EHumanWeaponState::Hands), MomentumSpeed(Constants::c_HumanBaseWalkingSpeed), bOnWall(false), bHoldingJumpButton(false),
WallNormal(FVector::Zero()), bCanFly(true)
{
    // Character doesnt have an object at start
    bHoldingObject = false;

    PrimaryActorTick.bCanEverTick = true;

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // Create a CameraComponent	
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    //Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
    Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    GetCharacterMovement()->AirControl = 1.0f; // Full control while in air
    GetCharacterMovement()->AirControlBoostMultiplier = 2.0f;
    GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.0f;
    GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
    GetCharacterMovement()->MaxWalkSpeed = Constants::c_ArmourBaseWalkingSpeed;
    GetCharacterMovement()->JumpZVelocity = Constants::c_ArmourBaseJumpHeight;
    GetCharacterMovement()->MaxWalkSpeedCrouched = Constants::c_ArmourBaseWalkingSpeed / 2;
    JumpMaxCount = 1;

    // Component Initialization

    LaserAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    LaserAudioComponent->SetupAttachment(RootComponent);

    LaserAudioComponent->bAutoActivate = false;
    LaserAudioComponent->bIsUISound = false;
    LaserAudioComponent->SetVolumeMultiplier(5.0f);

    BiopadComponent = CreateDefaultSubobject<UBiopadComponent>(TEXT("BiopadComponent"));
    FuelComponent = CreateDefaultSubobject<UFlyingTimerComponent>(TEXT("FuelComponent"));

    PlayerWallDetector = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PlayerWallDetector"));
    PlayerWallDetector->InitCapsuleSize(57.0f, 96.0f);
    PlayerWallDetector->BodyInstance.SetCollisionProfileName("OverlapAll");
    PlayerWallDetector->SetGenerateOverlapEvents(true);
    PlayerWallDetector->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    PlayerWallDetector->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Overlap);

    PlayerWallDetector->SetupAttachment(RootComponent);

    GetCharacterMovement()->SetPlaneConstraintNormal(FVector::UpVector);
}

TArray<AActor*> AGravityFPSTestCharacter::GetActorsInSphereFromCamera(float Radius, float TraceDist, float ConeAngle, FCollisionQueryParams Params)
{
    FVector ViewLocation;
    FRotator ViewRotation;
    GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

    FVector Forward = ViewRotation.Vector();
    ViewLocation += Forward * Constants::c_OffsetDistance;

    TArray<FHitResult> Hits;
    Params.AddIgnoredActor(this);

    float TraceDistance = TraceDist;
    FVector End = ViewLocation + Forward * TraceDistance;
    bool bHit = GetWorld()->SweepMultiByChannel(Hits, ViewLocation, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(Radius), Params);
    if (bHit)
    {
        TArray<AActor*> HitActors;
        for (const FHitResult& Hit : Hits)
        {
            HitActors.Add(Hit.GetActor());
        }
        return HitActors;
    }
    return TArray<AActor*>();
}
/// <summary>GetActorsInConeFromCamera is called by the homing projectile to determine which objects are within the player's line of sight</summary>
/// <param>Takes three float parameters, Radius, TraceDis, and ConeAngle which are pretty self explanatory</param>
/// <returns>return type is aTArray of AActor* that contains all actors found in the trace.</returns>
TArray<AActor*> AGravityFPSTestCharacter::GetActorsInConeFromCamera(float Radius, float TraceDist, float ConeAngle)
{
    TArray<AActor*> SeenActors;
    FVector ViewLocation;
    FRotator ViewRotation;
    GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

    FVector Forward = ViewRotation.Vector();
    ViewLocation += Forward * Constants::c_OffsetDistance;

    float TraceDistance = TraceDist;
    float ConeAngleDegrees = ConeAngle; // Controls how wide the cone is
    float CosConeAngle = FMath::Cos(FMath::DegreesToRadians(ConeAngleDegrees));

    FCollisionQueryParams RaycastParams;
    RaycastParams.AddIgnoredActor(this);
    UMissileManagerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMissileManagerSubsystem>();
    for (int32 i = Subsystem->ActiveMissiles.Num() - 1; i >= 0; --i)
    {
        if (Subsystem->ActiveMissiles[i].IsValid())
        {
            AMissileProjectile* Missile = Subsystem->ActiveMissiles[i].Get();
            RaycastParams.AddIgnoredActor(Missile);
        }
        else
        {
            Subsystem->ActiveMissiles.RemoveAt(i);
        }
    }
    TArray<AActor*> SphereActors = GetActorsInSphereFromCamera(Radius, TraceDist, ConeAngle, RaycastParams);

    // Filter based on cone angle

    for (AActor* Actor : SphereActors)
    {
        FVector ActorLocation = Actor->GetActorLocation();
        FVector DirectionToActor = (Actor->GetActorLocation() - ViewLocation).GetSafeNormal();
        float Dot = FVector::DotProduct(DirectionToActor, Forward);
        if (Dot >= CosConeAngle) // Check cone angle
        {
            // Line trace to check visibility
            FHitResult VisibilityHit;
            bool bBlocked = GetWorld()->LineTraceSingleByChannel(VisibilityHit, ViewLocation, ActorLocation, ECC_Visibility, RaycastParams);
            // Only add actor if not blocked
            if (!bBlocked || VisibilityHit.GetActor() == Actor)
            {
                SeenActors.Add(Actor);

                // Optional debug line
     //           DrawDebugLine(GetWorld(), ViewLocation, ActorLocation, FColor::Green, false, 1.0f, 0, 1.5f);
            }
            else
            {
                // Draw a red line to indicate it's blocked
     //           DrawDebugLine(GetWorld(), ViewLocation, ActorLocation, FColor::Red, false, 1.0f, 0, 1.5f);
            }
            if (bBlocked)
            {
    //        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Blocked by: %s"), *VisibilityHit.GetActor()->GetName()));
            }
            else
            {
      //     GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("No Block, Ray went through")));
            }
        }
    }


    // Debug visuals
 //   DrawDebugLine(GetWorld(), ViewLocation, End, FColor::Cyan, false, 1.0f, 0, 2.0f);
 //   DrawDebugSphere(GetWorld(), ViewLocation, 16.0f, 12, FColor::Blue, false, 1.0f);

    FVector ConeOrigin = ViewLocation;
    FVector ConeDirection = Forward;
    float Length = TraceDistance;
    float AngleWidthRadians = FMath::DegreesToRadians(ConeAngleDegrees);
#if 0
    // Draw the cone
    DrawDebugCone(
        GetWorld(),
        ConeOrigin,
        ConeDirection,
        Length,
        AngleWidthRadians,
        AngleWidthRadians,
        12,                     // Number of sides
        FColor::Red,
        false,                  // Persistent lines
        1.0f,                   // Lifetime
        0,                      // Depth priority
        1.0f                    // Thickness
    );
#endif
    return SeenActors;
}

void AGravityFPSTestCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();
    LaserAudioComponent->SetSound(LaserSoundBase);
    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        SavedLocation = GetActorLocation();
    }

    // Get any weapon component in the level, we don't care which one as long as there is at least one.
    GunReference = GetAnyWeaponComponent(GetWorld());
    if (GunReference)
    {
        GunReference->RegisterComponent();
    }
}

// TODO: add any new components that the player may have attached to their sockets to this function.
void AGravityFPSTestCharacter::HideSocketComponents()
{
    bHoldingObject = false;
    if (GunReference)
    {
        GunReference->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        GunReference->DetachWeapon(this);
        GunReference->SetVisibility(false, true); // hide all children, too
        GunReference->SetComponentTickEnabled(false);
        GunReference->ToggleFire(false);
    }
    if (CubeReference)
    {
        CubeReference->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        CubeReference->Destroy();
    }
}

void AGravityFPSTestCharacter::ShowSelectedSocketComponent()
{
    if (HumanWeapon == EHumanWeaponState::Gun)
    {
        if (GunReference)
        {
            GunReference->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GripPoint"));
            GunReference->AttachWeapon(this);
            GunReference->SetVisibility(true, true); // show all children, too
            GunReference->SetComponentTickEnabled(true);
            GunReference->ToggleFire(true);
            bHoldingObject = true;
        }
    }
    else if (HumanWeapon == EHumanWeaponState::EmergencyCube)
    {
#if 0
        CubeReference = GetWorld()->SpawnActor<ACubeProjectile>(CubeToSpawn, GetActorLocation(), GetActorRotation());
        if (CubeReference)
        {
            CubeReference->Hold(Mesh1P);
            CubeReference->GetCollisionComponent()->SetRelativeScale3D(FVector(0.2f));
        }
#endif
        //       bHoldingObject = false;
        }
    }

FName AGravityFPSTestCharacter::ConvertWeaponToString(int i)
{
    // returns a string based on the enumerated type that the player has equipped, represented as an integer.
    switch (i)
    {
    case 0:
    {
        if (bIsWearingArmour)
        {
            return "Laser";
        }
        else
        {
            return "Fist";
        }
    }
    case 1:
    {
        if (bIsWearingArmour)
        {
            return "Cube";
        }
        else
        {
            return "Gun";
        }
    }
    case 2:
    {
        if (bIsWearingArmour)
        {
            return "Missile";
        }
        else
        {
            return "Cube";
        }
    }
    case 3:
    {
        if (bIsWearingArmour)
        {
            return "Nuke";
        }
    }
    case 4:
    {
        if (bIsWearingArmour)
        {
            return "Invisibility";
        }
    }
    case 5:
    {
        if (bIsWearingArmour)
        {
            return "BioPad";
        }
    }
    default: return "";
    }
}

void AGravityFPSTestCharacter::WallDetect()
{
    FVector Start = PlayerWallDetector->GetComponentLocation();
    FVector End = Start; // just checking for overlaps.
    float CapsuleRadius = PlayerWallDetector->GetScaledCapsuleRadius();
    float CapsuleHalfHeight = PlayerWallDetector->GetScaledCapsuleHalfHeight();

    TArray<FHitResult> HitResults;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
        Params
    );

    ValidWallsInRange.Empty();

    FVector PlayerLocation = GetActorLocation();
    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (!bIsWearingArmour && HitActor && HitActor->ActorHasTag("WallRun") && GetCharacterMovement()->IsFalling() && bHoldingJumpButton)
            {
                bOnWall = true;
                GetCharacterMovement()->GravityScale = Constants::c_WallGravity;
                GetCharacterMovement()->bConstrainToPlane = true;
                ValidWallsInRange.Add(Hit);
            }
        }
    }

    // if ValidWallsInRange is empty then it means we are no longer colliding with any walls, so reset the player physics back to normal.
    if (ValidWallsInRange.IsEmpty())
    {
        ResetPhysics();
    }
    // if we have only one element, no need to find which one is closest, grab the normal and move on.
    else if (ValidWallsInRange.Num() == 1)
    {
        WallNormal = ValidWallsInRange[0].ImpactNormal;
    }
    else // we have more than one element in our array, so loop through them to figure out which one is the closest and use that normal.
    {
        FVector PlayerPosition = GetActorLocation();
        FVector ClosestDistance = PlayerPosition - ValidWallsInRange[0].ImpactPoint;
        FHitResult ClosestWall = ValidWallsInRange[0];
        for (int i = 1; i < ValidWallsInRange.Num(); i++)
        {
            FVector Distance = PlayerPosition - ValidWallsInRange[i].ImpactPoint;
            if (Distance.Size() < ClosestDistance.Size())
            {
                ClosestDistance = Distance;
                ClosestWall = ValidWallsInRange[i];
            }
        }
        WallNormal = ClosestWall.ImpactNormal;
    }
    if (bOnWall)
    {
        GetCharacterMovement()->SetPlaneConstraintNormal(WallNormal);
    }
}

UTP_WeaponComponent* AGravityFPSTestCharacter::GetAnyWeaponComponent(UWorld* World)
{
    if (!World) return nullptr;

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

    for (AActor* Actor : AllActors)
    {
        if (Actor)
        {
            UTP_WeaponComponent* WeaponComp = Actor->FindComponentByClass<UTP_WeaponComponent>();
            if (WeaponComp)
            {
                return WeaponComp; // Return the first one we find
            }
        }
    }

    return nullptr; // None found
}

void AGravityFPSTestCharacter::SpacebarReleased()
{
    if (!bIsWearingArmour && bOnWall && GetCharacterMovement()->IsFalling())
    {
        FVector JumpDirection = WallNormal;
        if (!(FMath::IsNearlyEqual(WallNormal.Z, 0.0f)))
        {
            // player is not colliding with the wall, they are instead colliding with the ceiling of a runnable wall.
            return;
        }
        JumpDirection.Normalize();

        FVector HorizontalPush = JumpDirection * Constants::c_HorizontalPush;
        FVector VerticalPush = FVector::UpVector * GetCharacterMovement()->JumpZVelocity;
        LaunchCharacter(HorizontalPush + VerticalPush, true, true);
        ResetPhysics();

        //     DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + WallNormal * 200.f, FColor::Red, false, 10.f, 0, 5.f);
        //     DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetCharacterMovement()->Velocity * 0.1f, FColor::Green, false, 10.f, 0, 5.f);

    }
}

void AGravityFPSTestCharacter::ResetPhysics()
{
    bOnWall = false;
    GetCharacterMovement()->bConstrainToPlane = false;
    GetCharacterMovement()->GravityScale = Constants::c_GravityScale;
}

/// <summary>IsTouchingAnySurface is called by Tick to check if the player is touching the ground or not</summary>
/// <param>Takes no parameters</param>
/// <returns>return type is bool. Returns true if the player has contact with any surface.</returns>
bool AGravityFPSTestCharacter::IsTouchingAnySurface()
{
    // Set up sweep parameters
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FVector Start = GetActorLocation();
    FVector End = Start; // Not moving — just checking overlap

    UCapsuleComponent* Capsule = GetCapsuleComponent();
    float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
    float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();

    FHitResult HitResult;
    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        Start,
        End,
        GetActorQuat(),
        ECC_Visibility,
        FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight + 10.0f),
        Params
    );

    // Draw debug capsule
#if 0
    DrawDebugCapsule(
        GetWorld(),
        Start,
        CapsuleHalfHeight,
        CapsuleRadius,
        FQuat::Identity,
        bHit ? FColor::Green : FColor::Red,
        false, 2.0f
    );
#endif
    return bHit;
}

/// <summary>PlayerLaserSound is called by input from the player - it starts a sound effect that is activated when the left mouse button is clicked</summary>
/// <param>Takes no parameters</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::PlayLaserSound()
{
    if (bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::Laser)
    {
        if (LaserAudioComponent && !LaserAudioComponent->IsPlaying())
        {
            LaserAudioComponent->Play();
        }
    }
}

/// <summary>StopLaserSound is called by input from the player - it ends the sound effect that is currently playing when the left mouse button is released</summary>
/// <param>Takes no parameters</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::StopLaserSound()
{
    if (LaserAudioComponent && LaserAudioComponent->IsPlaying())
    {
        LaserAudioComponent->Stop();
    }
}

void AGravityFPSTestCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

#if 0
    // Draw a debug capsule matching the PlayerWallDetector
    DrawDebugCapsule(
        GetWorld(),
        PlayerWallDetector->GetComponentLocation(),                       // Center
        PlayerWallDetector->GetScaledCapsuleHalfHeight(),                 // Half height
        PlayerWallDetector->GetScaledCapsuleRadius(),                     // Radius
        PlayerWallDetector->GetComponentQuat(),                           // Rotation
        FColor::Green,                                                    // Color
        false,                                                             // Persistent lines
        -1.0f,                                                             // Lifetime
        0,                                                                 // Depth priority
        1.0f                                                               // Line thickness
    );
    // Draw a debug capsule matching the player's capsule size.
    DrawDebugCapsule(
        GetWorld(),
        GetCapsuleComponent()->GetComponentLocation(),                       // Center
        GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),                 // Half height
        GetCapsuleComponent()->GetScaledCapsuleRadius(),                     // Radius
        GetCapsuleComponent()->GetComponentQuat(),                           // Rotation
        FColor::Red,                                                    // Color
        false,                                                             // Persistent lines
        -1.0f,                                                             // Lifetime
        0,                                                                 // Depth priority
        1.0f                                                               // Line thickness
    );
#endif

    FVector p = GetActorLocation();
    GEngine->AddOnScreenDebugMessage(20, 5.0f, FColor::Green, FString::Printf(TEXT("Player Position =  %f, %f, %f"), p.X, p.Y, p.Z));

    // Armour Behaviour

    if (bIsWearingArmour && bIsThrusting && bCanFly)
    {
        // Disable gravity while thrusting
        GetCharacterMovement()->GravityScale = 0.f;

        // If no key is being held down, reset the thrust input to 0 to ensure that stored velocity isn't used again after landing. 
        if (!bIsKeyDown)
        {
            CurrentThrustInput = FVector::ZeroVector;
        }
        if (GetCharacterMovement()->Velocity.Z < 0)
        {
            FVector CancelFall = GetCharacterMovement()->Velocity;
            CancelFall.Z = 0;
            GetCharacterMovement()->Velocity = CancelFall;
        }

        // Accelerate toward input
        FlyingVelocity += CurrentThrustInput.GetSafeNormal() * Constants::c_ThrustAccel * DeltaTime;

        // Clamp to max flying speed
        FlyingVelocity = FlyingVelocity.GetClampedToMaxSize(Constants::c_MaxFlyingSpeed);

        // Apply custom flying velocity
        GetCharacterMovement()->Velocity = FlyingVelocity;

        if (GEngine)
        {
            //   GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Current Velocity = (%f, %f, %f)"), FlyingVelocity.X, FlyingVelocity.Y, FlyingVelocity.Z));
            //   GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("ThrustInput = %f, %f"), CurrentThrustInput.X, CurrentThrustInput.Y));
        }
    }
    else if (bIsWearingArmour)
    {
        // Reset for ground movement
        GetCharacterMovement()->GravityScale = Constants::c_GravityScale;
        if (IsTouchingAnySurface())
        {
            FlyingVelocity = FVector::ZeroVector;
        }
    }

    if (bIsWearingArmour && bIsInvisible)
    {
        InvisibilityTimer -= DeltaTime;
        // Triggers when invisibility reaches 0 (obvious), but more importantly also when the player attempts to swap to other items while
        // invisible. This can be adjusted for testing to not occur by removing the last or block. (The second one should never go away).
        if (InvisibilityTimer <= 0.0f || !bIsWearingArmour || !(ArmouredWeapon == EArmourWeaponState::Invisibility))
        {
            bIsInvisible = false;
            AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            AGravityFPSTestPlayerController* pPlayerController = Cast<AGravityFPSTestPlayerController>(pController);
            pPlayerController->HideInvisibilityWidget();
            if (InvisibilityTimer <= 0.0f)
            {
                // We set InvisibilityTimer to be 0 here to avoid visual glitch issues.
                InvisibilityTimer = 0.0f;
            }
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Invisibility Time = %f"), InvisibilityTimer));
        }
    }

    // Human Behaviour

    if (!bIsWearingArmour)
    {
        FVector HorizontalVelocity = GetVelocity();
        HorizontalVelocity.Z = 0.0f;
        float HorizontalSpeed = HorizontalVelocity.Size();
        if (!(FMath::IsNearlyEqual(HorizontalSpeed, MomentumSpeed)))
        {
            if (HorizontalSpeed < Constants::c_HumanBaseWalkingSpeed)
            {
                MomentumSpeed = Constants::c_HumanBaseWalkingSpeed;
            }
            else
            {
                MomentumSpeed = HorizontalSpeed;
            }
        }
        if (bIsKeyDown && HorizontalSpeed != 0.0f)
        {
            float pct = (MomentumSpeed - Constants::c_HumanBaseWalkingSpeed) / (Constants::c_HumanMaxWalkingSpeed - Constants::c_HumanBaseWalkingSpeed);
            // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("pct = %f"), pct));
            float LerpedValue = FMath::Lerp(Constants::c_HumanBaseJumpHeight, Constants::c_HumanMaxJumpHeight, pct);
            GetCharacterMovement()->JumpZVelocity = LerpedValue;
            if (GetCharacterMovement()->IsFalling())
            {
                WallDetect(); // this function will handle any wall running logic as necessary.
            }
            else if (bOnWall)
            {
                ResetPhysics();
            }
        }
    }
    }

/// <summary>Move is called by input from the player</summary>
/// <param>Takes an FInputActionValue as a parameter. This value can be read as a 2DVector with values of -1, 0, or 1 
/// to represent which keys are being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();
    bIsKeyDown = true;
    bIHaveMoved = true;

    if (Controller != nullptr)
    {
        // Save thrust input for use in Tick
        if (bIsWearingArmour && bIsThrusting)
        {
            FVector Forward = FirstPersonCameraComponent->GetForwardVector();
            FVector Right = FirstPersonCameraComponent->GetRightVector();
            CurrentThrustInput = (Forward * MovementVector.Y) + (Right * MovementVector.X);
        }
        else
        {
            // Ground movement
            AddMovementInput(GetActorForwardVector(), MovementVector.Y);
            AddMovementInput(GetActorRightVector(), MovementVector.X);

            if (!bIsWearingArmour && !GetCharacterMovement()->IsFalling())
            {
                MomentumSpeed += 1.0f;
                MomentumSpeed = FMath::Clamp(MomentumSpeed, Constants::c_HumanBaseWalkingSpeed, Constants::c_HumanMaxWalkingSpeed);
                GetCharacterMovement()->MaxWalkSpeed = MomentumSpeed;
            }
        }

    }
}

/// <summary>Look is called by input from the player</summary>
/// <param>Takes an FInputActionValue as a parameter. This value can be read as a 2DVector with values of -1, 0, or 1 
/// to represent which way the mouse is being moved</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

/// <summary>Crouch is called by input from the player - it activates when the left shift key is pressed. Crouching is only allowed when the player is not wearing
/// armour. Pressing the left shift key while wearing armour will initiate flying instead of crouching. </summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a single float to determine if the designated is key is being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::StartCrouching(const FInputActionValue& Value)
{
    if (!bIsWearingArmour)
    {
        Crouch();
    }
}

/// <summary>StopCrouching is called by input from the player - it activates when the left shift key is released.</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being released</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::StopCrouching(const FInputActionValue& Value)
{
    UnCrouch();
}

/// <summary>StartThrusters is called by input from the player - it activates when the left shift key is pressed, allowing the player to fly.
/// Flying is only allowed while the player is wearing armour. Pressing the left shift key while unarmoured will cause the player to crouch instead
/// of fly.</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::StartThrusters(const FInputActionValue& Value)
{
    if (bIsWearingArmour && !bIsCrouched && bCanFly)
    {
        bIsThrusting = true;
        if (FuelComponent)
        {
            FuelComponent->UpdateFlightState(true);
        }
        GetCharacterMovement()->DoJump(false);

        // Apply that yaw rotation to the stored velocity
        if (!FlyingVelocity.IsNearlyZero())
        {
            // Find the delta between the previous yaw rotation and the current one.
            float YawDelta = GetActorRotation().Yaw - RotationWhenThrustersDisabled.Yaw;

            FRotator DeltaRotator(0.f, YawDelta, 0.f);
            FlyingVelocity = DeltaRotator.RotateVector(FlyingVelocity);
        }
    }
}

/// <summary>EndThrusters is called by input from the player - it activates when the left shift key is released.</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being released</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::EndThrusters(const FInputActionValue& Value)
{

    bIsThrusting = false;
    if (FuelComponent)
    {
        FuelComponent->UpdateFlightState(false);
    }
    RotationWhenThrustersDisabled = GetActorRotation();

}

/// <summary>ShootLasers is called by input from the player - it activates when the player left clicks. This function is called every frame the left
/// click is held down, and fires continuous projectiles while they do so.</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::ShootLasers(const FInputActionValue& Value)
{
    if (bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::Laser)
    {
        float TimeInSeconds = GetWorld()->GetTimeSeconds();
        if (TimeInSeconds - TimeSinceLastShot > Constants::c_MissileSpawnDelay)
        {
            TimeSinceLastShot = TimeInSeconds;
            FVector SpawnLocation = GetActorLocation() + FirstPersonCameraComponent->GetForwardVector() * Constants::c_SpawnOffset;
            FRotator MyRotation = FirstPersonCameraComponent->GetComponentRotation();
            ALaserBeamProjectile* SpawnedLaser = GetWorld()->SpawnActor<ALaserBeamProjectile>(LaserToSpawn, SpawnLocation, MyRotation);
            if (SpawnedLaser)
            {
                FVector PlayerVelocity = GetVelocity();
                SpawnedLaser->AddVelocity(PlayerVelocity);
            }
        }
    }
}
/// <summary>CycleAbilityByMouse is called by input from the player - it activates when the player scrolls the mouse wheel. It allows the
/// player to cycle forwards or backwards through their list of abilities to quickly select the one they would most like to use</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a single float which can have a value of 1, 0, or -1 to determine
/// which way the mouse wheel is being scrolled.</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::CycleAbilityByMouse(const FInputActionValue& Value)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && !PC->IsInputKeyDown(EKeys::LeftMouseButton) && !PC->IsInputKeyDown(EKeys::RightMouseButton))
    {
        float ScrollDelta = Value.Get<float>();
        if (FMath::IsNearlyZero(ScrollDelta))
            return;

        int32 CurrentIndex = 0;
        int32 MaxIndex = 0;
        HideSocketComponents();
        if (bIsWearingArmour)
        {
            CurrentIndex = static_cast<int32>(ArmouredWeapon);
            MaxIndex = static_cast<int32>(EArmourWeaponState::MAX) - 1; // exclude Armour MAX
        }
        else
        {
            CurrentIndex = static_cast<int32>(HumanWeapon);
            MaxIndex = static_cast<int32>(EHumanWeaponState::MAX) - 1; // exclude Human MAX
        }

        if (ScrollDelta > 0)
        {
            CurrentIndex++;
            if (CurrentIndex > MaxIndex)
                CurrentIndex = 0; // Wrap around to first
        }
        else
        {
            CurrentIndex--;
            if (CurrentIndex < 0)
                CurrentIndex = MaxIndex; // Wrap around to last
        }
        if (bIsWearingArmour)
        {
            ArmouredWeapon = static_cast<EArmourWeaponState>(CurrentIndex);
        }
        else
        {
            HumanWeapon = static_cast<EHumanWeaponState>(CurrentIndex);
            ShowSelectedSocketComponent();
        }

        SwitchEquipment(ConvertWeaponToString(CurrentIndex));
        //        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Current Ability = %d"), CurrentIndex));
    }
}

/// <summary>DropCube is called by input from the player when the player left clicks. The closest object will be teleported to the most recently
/// saved location. If the player has not manually saved a location through the SaveLocation function, then the saved location will be the location in
/// which the player started the game. (Refer to the Constructor to see the initialization of SavedLocation). The logic of the teleportation itself
/// is handled inside of CubeProjectile class</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::DropCube(const FInputActionValue& Value)
{
    if ((bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::EmergencyCube)
        || (!bIsWearingArmour && HumanWeapon == EHumanWeaponState::EmergencyCube))
    {
        FVector SpawnLocation = GetActorLocation() + FirstPersonCameraComponent->GetForwardVector() * Constants::c_SpawnOffset;
        FRotator MyRotation = FirstPersonCameraComponent->GetComponentRotation();
        // adding ActorSpawnParameters to reduce the likelihood of cubes colliding into themselves and exploding.
        FActorSpawnParameters ActorSpawnParams;
        ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
        ACubeProjectile* SpawnedCube = GetWorld()->SpawnActor<ACubeProjectile>(CubeToSpawn, SpawnLocation, MyRotation, ActorSpawnParams);
        if (SpawnedCube)
        {
            // IMPORTANT: BY DEFAULT CUBEPROJECTILE DOES NOT HAVE TICK ENABLED. THIS COULD BE A CONSTRUCTOR PARAMETER INSTEAD, IF UNREAL WOULD
            // ALLOW ME TO USE IT INSTEAD OF BEING AN IDIOT. IF YOU WANT THE CUBE TO HAVE TICK ENABLED, YOU MUST SET IT EXPLICITLY AFTER CREATION.
            SpawnedCube->SetActorTickEnabled(true);
            FVector PlayerVelocity = GetVelocity();
            SpawnedCube->AddVelocity(PlayerVelocity);
        }
    }
}

/// <summary>SaveLocation is called by input from the player when the player right clicks. If the current weapon equipped is the emergency cube,
/// then the position in which the player is currently standing will become the most recently saved location, used when the player activates the emergency
/// cube with a left click. It is impossible to save more than one location, and saving a new location overwrites the old one.</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::SaveLocation()
{
    if ((bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::EmergencyCube)
        || (!bIsWearingArmour && HumanWeapon == EHumanWeaponState::EmergencyCube))
    {
        SavedLocation = GetActorLocation();
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Saved Position = %f, %f, %f"), SavedLocation.X, SavedLocation.Y, SavedLocation.Z));
    }
}

/// <summary>FireMissile is called by input from the player when the player left clicks. It spawns a missile that will home in on the closest target.
/// The logic of the missile homing itself is handled inside of the MissileProjectile class. When determining a line of sight, the MissileProjectile
/// class will call this class's GetActorsInConeFromCamera() function to determine which objects are within the player's line of sight, if any. </summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::FireMissile()
{
    if (bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::Missile)
    {
        FVector SpawnLocation = GetActorLocation() + FirstPersonCameraComponent->GetForwardVector() * Constants::c_SpawnOffset;
        FRotator MyRotation = FirstPersonCameraComponent->GetComponentRotation();
        AMissileProjectile* SpawnedMissile = GetWorld()->SpawnActor<AMissileProjectile>(MissileToSpawn, SpawnLocation, MyRotation);
        if (SpawnedMissile)
        {
            FVector PlayerVelocity = GetVelocity();
            SpawnedMissile->AddVelocity(PlayerVelocity);
        }
    }
}

/// <summary>ChargeNuke is called by input from the player - it is active while the left mouse button is being held down. While held, the player begins
/// to charge up a ranged projectile that they will prepare to throw. Once the button is released, the NukeCharge value is passed onto the FireNuke function
/// to determine final velocity.</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::ChargeNuke()
{
    if (bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::TankRifle)
    {
        NukeCharge += Constants::c_NukeChargeRate;
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Charge Rate = %f"), NukeCharge));
    }
}

/// <summary>FireNuke is called by input from the player - it activates when the left mouse button is released. A nuke object is spawned and thrown in
/// front of the player. The nuke destroys any object it collides with, (except the floor), and pushes all surrounding objects backwards.
/// The logic for the nuke itself is handled in the TankRifleProjectile class.</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::FireNuke()
{
    if (bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::TankRifle)
    {
        FVector Forwards = FirstPersonCameraComponent->GetForwardVector();
        FVector SpawnLocation = GetActorLocation() + Forwards * Constants::c_SpawnOffset;
        FRotator MyRotation = FirstPersonCameraComponent->GetComponentRotation();
        ATankRifleProjectile* SpawnedNuke = GetWorld()->SpawnActor<ATankRifleProjectile>(NukeToSpawn, SpawnLocation, MyRotation);
        if (SpawnedNuke)
        {
            FVector PlayerVelocity = GetVelocity();
            FVector NukeVelocity = Forwards * FMath::Clamp(NukeCharge, 0.0f, Constants::c_NukeMaxCharge);
            SpawnedNuke->AddVelocity(PlayerVelocity + NukeVelocity);
        }
        NukeCharge = 0.0f;
    }
}

/// <summary>ToggleInvisibility is called by input from the player - it activates when the left click is pressed. the boolean determining
/// the player's visibility is flipped, and the widget displaying the time remaining for invisibility is either revealed or hidden respectively.</summary>
/// <param>Takes an FInputActionValue as a parameter. This value is read as a boolean to determine if the designated is key is being pressed</param>
/// <returns>return type is void</returns>
void AGravityFPSTestCharacter::ToggleInvisibility()
{
    AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AGravityFPSTestPlayerController* pPlayerController = Cast<AGravityFPSTestPlayerController>(pController);

    if (bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::Invisibility && !bIsInvisible)
    {
        bIsInvisible = true;
        pPlayerController->ShowInvisibilityWidget();
        Mesh1P->SetVisibility(false);
        GetMesh()->SetVisibility(false, true);
    }
    else if (bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::Invisibility && bIsInvisible)
    {
        bIsInvisible = false;
        pPlayerController->HideInvisibilityWidget();
        Mesh1P->SetVisibility(true);
        GetMesh()->SetVisibility(true, true);
    }
}

void AGravityFPSTestCharacter::ScanObject()
{
    // TODO: Make sure that you allow the Biopad to also be used when not wearing Armour if it's equipped.
    if (bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::Biopad)
    {
        BiopadComponent->TrySelect();
    }
}

void AGravityFPSTestCharacter::RemoveLastSelectedObject()
{
    // TODO: Make sure that you allow the Biopad to also be used when not wearing Armour if it's equipped.
    if (bIsWearingArmour && ArmouredWeapon == EArmourWeaponState::Biopad)
    {
        BiopadComponent->RemoveLastSelected();
    }
}

void AGravityFPSTestCharacter::DetectDoor()
{
    float TraceDistance = Constants::c_DoorDetectionRange;
    FVector ViewLocation;
    FRotator ViewRotation;
    GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);
    FHitResult HitResult;
    FVector Forward = ViewRotation.Vector();
    FVector End = ViewLocation + Forward * TraceDistance;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, End, ECC_Visibility, Params);
    if (bHit && HitResult.GetActor() && HitResult.GetActor()->Implements<UDoorInterface>())
    {
        IDoorInterface::Execute_Interact(HitResult.GetActor());
    }
    else if (HitResult.GetActor())
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor %s does not implement interface"),
            *HitResult.GetActor()->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No actor hit"));
    }
    // Always draw the line to see where the trace goes
    DrawDebugLine(
        GetWorld(),
        ViewLocation,   // trace start
        End,            // trace end
        FColor::Green,  // line color
        false,          // persistent (false = disappears after time)
        2.0f,           // lifetime (seconds)
        0,              // depth priority
        1.0f            // thickness
    );

    // If hit something, draw a red point
    if (bHit)
    {
        DrawDebugPoint(
            GetWorld(),
            HitResult.ImpactPoint,
            10.0f,       // size
            FColor::Red, // point color
            false,
            2.0f
        );
    }

}

void AGravityFPSTestCharacter::SwapArmour()
{
    bIsWearingArmour = !bIsWearingArmour;
    if (bIsWearingArmour)
    {
        GetCharacterMovement()->MaxWalkSpeed = Constants::c_ArmourBaseWalkingSpeed;
        GetCharacterMovement()->JumpZVelocity = Constants::c_ArmourBaseJumpHeight;
        GetCharacterMovement()->MaxWalkSpeedCrouched = Constants::c_ArmourBaseWalkingSpeed / 2;
        SwitchEquipment(ConvertWeaponToString(static_cast<int32>(ArmouredWeapon)));
    }
    else if (!bIsWearingArmour)
    {
        GetCharacterMovement()->MaxWalkSpeed = Constants::c_HumanBaseWalkingSpeed;
        GetCharacterMovement()->JumpZVelocity = Constants::c_HumanBaseJumpHeight;
        GetCharacterMovement()->MaxWalkSpeedCrouched = Constants::c_HumanBaseWalkingSpeed / 2;
        SwitchEquipment(ConvertWeaponToString(static_cast<int32>(HumanWeapon)));
    }

    ResetPhysics();
    HideSocketComponents();
    if (!bIsWearingArmour)
    {
        ShowSelectedSocketComponent();
    }
}

void AGravityFPSTestCharacter::SetHoldingObject(bool bNewHasRifle)
{
    bHoldingObject = bNewHasRifle;
}

bool AGravityFPSTestCharacter::GetHoldObjectState()
{
    return bHoldingObject;
}