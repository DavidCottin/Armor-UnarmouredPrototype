// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileManager.generated.h"

UCLASS()
class GRAVITYFPSTEST_API UMissileManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	
	UPROPERTY()
	TArray<TWeakObjectPtr<class AMissileProjectile>> ActiveMissiles;

	void RegisterMissile(AMissileProjectile* Missile) { ActiveMissiles.Add(Missile); };

	void UnregisterMissile(AMissileProjectile* Missile) { ActiveMissiles.Remove(Missile); };

};
