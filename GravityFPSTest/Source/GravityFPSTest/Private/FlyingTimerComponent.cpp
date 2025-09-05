// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingTimerComponent.h"
#include "Constants.h"
#include "FuelWidget.h"
#include "GravityFPSTest/GravityFPSTestPlayerController.h"
#include "GravityFPSTest/GravityFPSTestCharacter.h"

// Sets default values for this component's properties
UFlyingTimerComponent::UFlyingTimerComponent() : bIsThrusting(false), TimeElapsedWhileThrusting(false), Fuel(Constants::c_MaxFuelCapacity)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UFlyingTimerComponent::UpdateFlightState(bool b)
{
	bIsThrusting = b;
	if (bIsThrusting)
	{
		TimeElapsedWhileThrusting = 0.0f;
	}
}

void UFlyingTimerComponent::ResetMaxFuel()
{
	Fuel = Constants::c_MaxFuelCapacity;
	UpdateFuel();
}

// Called when the game starts
void UFlyingTimerComponent::BeginPlay()
{
	Super::BeginPlay();
	 if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	 {
		 Player = Cast<AGravityFPSTestPlayerController>(OwnerPawn->GetController());
		 Character = Cast<AGravityFPSTestCharacter>(OwnerPawn);
	 }
	// ...
	
}

void UFlyingTimerComponent::UpdateFuel()
{
	float PCT = Fuel / Constants::c_MaxFuelCapacity;
	if (Player)
	{
		if (PCT <= 0.0f)
		{
			PCT = 0.0f;
			bIsThrusting = false;
			Character->SetFlightAbility(false);
		}
		Player->GetFuelWidget()->SetBarPercent(PCT);
	}
}

// Called every frame
void UFlyingTimerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsThrusting)
	{
		TimeElapsedWhileThrusting += DeltaTime;
		TimeElapsedWhileThrusting = FMath::Clamp(TimeElapsedWhileThrusting, 0.0f, Constants::c_MaxFuelOptimizationLevel);
		if (TimeElapsedWhileThrusting != 0.0f)
		{
			float ConsumptionRate = Constants::c_FuelCost / TimeElapsedWhileThrusting;
			Fuel -= ConsumptionRate;
			UpdateFuel();
		}
	}
	else if (Fuel >= 0.0f)
	{
		Character->SetFlightAbility(true);
		TimeElapsedWhileThrusting = 0.0f;
	}
}

