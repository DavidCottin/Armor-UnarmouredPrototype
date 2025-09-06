// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlyingTimerComponent.generated.h"

class AGravityFPSTestPlayerController;
class AGravityFPSTestCharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRAVITYFPSTEST_API UFlyingTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFlyingTimerComponent();
	void UpdateFlightState(bool b);
	void ResetMaxFuel();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool bIsThrusting;
	float TimeElapsedWhileThrusting;
	float Fuel;
	AGravityFPSTestPlayerController* Player;
	AGravityFPSTestCharacter* Character;

	void UpdateFuel();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
