// Copyright Epic Games, Inc. All Rights Reserved.


#include "GravityFPSTestPlayerController.h"
#include "GravityFPSTestCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Public/HelmetUserWidget.h"
#include "Public/InvisibilityHUDUserWidget.h"
#include "Public/RadarUserWidget.h"
#include "Public/RadarMap.h"
#include "Public/BiopadUserWidget.h"
#include "Public/FuelWidget.h"
#include "Public/FlyingTimerComponent.h"
#include "BiopadComponent.h"
#include "IconsUserWidget.h"

void AGravityFPSTestPlayerController::BeginPlay()
{
    Super::BeginPlay();

    MyCharacter->OnEquipmentChanged.AddDynamic(this, &AGravityFPSTestPlayerController::HandleEquipmentChanged);

    // get the enhanced input subsystem
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // add the mapping context so we get controls
        Subsystem->AddMappingContext(InputMappingContext, 0);

        UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));

        if (HelmetWidgetClass)
        {
            HelmetWidget = CreateWidget<UHelmetUserWidget>(GetWorld(), HelmetWidgetClass);
            if (HelmetWidget)
            {
                HelmetWidget->AddToViewport();
                if (MyCharacter && MyCharacter->IsWearingArmour())
                {
                    HelmetWidget->PlayFadeIn();
                }
            }
        }

        if (InvisibilityWidgetClass)
        {
            InvisibilityWidget = CreateWidget<UInvisibilityHUDUserWidget>(GetWorld(), InvisibilityWidgetClass);
            if (InvisibilityWidget)
            {
                InvisibilityWidget->AddToViewport();
                InvisibilityWidget->SetVisibility(ESlateVisibility::Hidden);
            }
        }

        if (RadarWidgetClass)
        {
            RadarWidget = CreateWidget<URadarUserWidget>(GetWorld(), RadarWidgetClass);
            if (RadarWidget)
            {
                RadarWidget->AddToViewport();
                if (MyCharacter && !MyCharacter->IsWearingArmour())
                {
                    RadarWidget->SetVisibility(ESlateVisibility::Hidden);
                }
            }
        }

        if (BiopadWidgetClass)
        {
            BiopadWidget = CreateWidget<UBiopadUserWidget>(GetWorld(), BiopadWidgetClass);
            if (BiopadWidget)
            {
                BiopadWidget->AddToViewport();
            }
        }

        if (FuelWidgetClass)
        {
            FuelWidget = CreateWidget<UFuelWidget>(GetWorld(), FuelWidgetClass);
            if (FuelWidget)
            {
                FuelWidget->AddToViewport();
                if (MyCharacter && !MyCharacter->IsWearingArmour())
                {
                    FuelWidget->SetVisibility(ESlateVisibility::Hidden);
                }
            }
        }

        if (IconsWidgetClass)
        {
            IconWidget = CreateWidget<UIconsUserWidget>(GetWorld(), IconsWidgetClass);
            if (IconWidget)
            {
                IconWidget->AddToViewport();
            }
        }
    }
}

void AGravityFPSTestPlayerController::HandleEquipmentChanged(FName name)
{
    if (IconWidget)
    {
        IconWidget->SetDisplayLabel(name);
    }
}

void AGravityFPSTestPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGravityFPSTestPlayerController::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGravityFPSTestPlayerController::StopJumping);

        // Crouching
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AGravityFPSTestPlayerController::StartCrouching);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AGravityFPSTestPlayerController::StopCrouching);

        // Flying
        EnhancedInputComponent->BindAction(FlyAction, ETriggerEvent::Started, this, &AGravityFPSTestPlayerController::StartThrusters);
        EnhancedInputComponent->BindAction(FlyAction, ETriggerEvent::Completed, this, &AGravityFPSTestPlayerController::EndThrusters);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::Move);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AGravityFPSTestPlayerController::KeyUp);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::Look);

        // Attacking
        EnhancedInputComponent->BindAction(LaserAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::ShootLasers);
        EnhancedInputComponent->BindAction(LaserAction, ETriggerEvent::Started, this, &AGravityFPSTestPlayerController::PlayLaserSound);
        EnhancedInputComponent->BindAction(LaserAction, ETriggerEvent::Completed, this, &AGravityFPSTestPlayerController::StopLaserSound);

        // Scrolling
        EnhancedInputComponent->BindAction(ScrollWheelAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::CycleAbilityByMouse);

        // EmergencyCube
        EnhancedInputComponent->BindAction(DefaultRightClickAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::SaveLocation);
        EnhancedInputComponent->BindAction(DefaultLeftClickAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::DropCube);

        // Missile
        EnhancedInputComponent->BindAction(DefaultLeftClickAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::FireMissile);

        // Tank Rifle
        EnhancedInputComponent->BindAction(TankRifleAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::ChargeNuke);
        EnhancedInputComponent->BindAction(TankRifleAction, ETriggerEvent::Completed, this, &AGravityFPSTestPlayerController::FireNuke);

        // Invisibility
        EnhancedInputComponent->BindAction(DefaultLeftClickAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::ToggleInvisibility);

        // Biopad
        EnhancedInputComponent->BindAction(DefaultLeftClickAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::ScanObject);
        EnhancedInputComponent->BindAction(DefaultRightClickAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::RemoveLastSelectedObject);
        EnhancedInputComponent->BindAction(DefaultMiddleClickAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::SwitchBiopadDisplay);

        // Swap
        EnhancedInputComponent->BindAction(SwapAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::SwapArmour);

        // Interact
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AGravityFPSTestPlayerController::DetectDoor);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
    }
}

void AGravityFPSTestPlayerController::Move(const FInputActionValue& Value)
{
    MyCharacter->Move(Value);
}

void AGravityFPSTestPlayerController::KeyUp()
{
    MyCharacter->KeyUp();
}

void AGravityFPSTestPlayerController::Look(const FInputActionValue& Value)
{
    MyCharacter->Look(Value);
}

void AGravityFPSTestPlayerController::Jump(const FInputActionValue& Value)
{
    MyCharacter->Jump();
    MyCharacter->JumpKeyPressed(true);
}

void AGravityFPSTestPlayerController::StopJumping(const FInputActionValue& Value)
{
    MyCharacter->StopJumping();
    MyCharacter->SpacebarReleased();
    MyCharacter->JumpKeyPressed(false);
}

void AGravityFPSTestPlayerController::StartCrouching(const FInputActionValue& Value)
{
    MyCharacter->StartCrouching(Value);
}

void AGravityFPSTestPlayerController::StopCrouching(const FInputActionValue& Value)
{
    MyCharacter->StopCrouching(Value);
}

void AGravityFPSTestPlayerController::StartThrusters(const FInputActionValue& Value)
{
    MyCharacter->StartThrusters(Value);
}

void AGravityFPSTestPlayerController::EndThrusters(const FInputActionValue& Value)
{
    MyCharacter->EndThrusters(Value);
}

void AGravityFPSTestPlayerController::ShootLasers(const FInputActionValue& Value)
{
    MyCharacter->ShootLasers(Value);
}

void AGravityFPSTestPlayerController::PlayLaserSound()
{
    MyCharacter->PlayLaserSound();
}

void AGravityFPSTestPlayerController::StopLaserSound()
{
    MyCharacter->StopLaserSound();
}

void AGravityFPSTestPlayerController::CycleAbilityByMouse(const FInputActionValue& Value)
{
    MyCharacter->CycleAbilityByMouse(Value);
}

void AGravityFPSTestPlayerController::DropCube(const FInputActionValue& Value)
{
    MyCharacter->DropCube(Value);
}

void AGravityFPSTestPlayerController::SaveLocation()
{
    MyCharacter->SaveLocation();
}

void AGravityFPSTestPlayerController::FireMissile()
{
    MyCharacter->FireMissile();
}

void AGravityFPSTestPlayerController::ChargeNuke()
{
    MyCharacter->ChargeNuke();
}

void AGravityFPSTestPlayerController::FireNuke()
{
    MyCharacter->FireNuke();
}

void AGravityFPSTestPlayerController::ToggleInvisibility()
{
    MyCharacter->ToggleInvisibility();
}

void AGravityFPSTestPlayerController::ScanObject()
{
    MyCharacter->ScanObject();
}

void AGravityFPSTestPlayerController::RemoveLastSelectedObject()
{
    MyCharacter->RemoveLastSelectedObject();
}

void AGravityFPSTestPlayerController::SwitchBiopadDisplay()
{
    BiopadWidget->ToggleDistance();
}

void AGravityFPSTestPlayerController::DetectDoor()
{
    MyCharacter->DetectDoor();
}

void AGravityFPSTestPlayerController::SwapArmour()
{
    MyCharacter->SwapArmour();
    FTimerHandle RadarDelayHandle;
    FTimerHandle FuelDelayHandle;
    if (MyCharacter->IsWearingArmour())
    {
        HelmetWidget->PlayFadeIn();
        GetWorldTimerManager().SetTimer(
            RadarDelayHandle,
            this,                            
            &AGravityFPSTestPlayerController::ShowRadar,
            0.5f,                             // Delay in seconds
            false                             // Do not loop (run only once)
        );
        GetWorldTimerManager().SetTimer(
            FuelDelayHandle,
            this,
            &AGravityFPSTestPlayerController::ShowFuelWidget,
            0.5f,                             
            false                             
        );
        // forcefully end the players thrusters so that they must release the button and press it a second time if they wish to fly.
        MyCharacter->EndThrusters(true);
    }
    else
    {
        HelmetWidget->PlayFadeOut();
        GetWorldTimerManager().SetTimer(
            RadarDelayHandle,
            this,                             
            &AGravityFPSTestPlayerController::HideRadar, 
            0.5f,                             // Delay in seconds
            false                             // Do not loop (run only once)
        );

        GetWorldTimerManager().SetTimer(
            FuelDelayHandle,
            this,
            &AGravityFPSTestPlayerController::HideFuelWidget,
            0.5f,                             
            false
        );
        InvisibilityWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AGravityFPSTestPlayerController::ShowInvisibilityWidget()
{
    if (InvisibilityWidget)
    {
        InvisibilityWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void AGravityFPSTestPlayerController::HideInvisibilityWidget()
{
    if (InvisibilityWidget)
    {
        InvisibilityWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AGravityFPSTestPlayerController::ShowFuelWidget()
{
    if (FuelWidget)
    {
        FuelWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void AGravityFPSTestPlayerController::HideFuelWidget()
{
    if (FuelWidget)
    {
        FuelWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AGravityFPSTestPlayerController::ShowRadar()
{
    if (RadarWidget)
    {
        RadarWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void AGravityFPSTestPlayerController::HideRadar()
{
    if (RadarWidget)
    {
        RadarWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AGravityFPSTestPlayerController::AcknowledgePossession(APawn* InPawn)
{
    Super::AcknowledgePossession(InPawn);
    MyCharacter = Cast<AGravityFPSTestCharacter>(InPawn);
}
