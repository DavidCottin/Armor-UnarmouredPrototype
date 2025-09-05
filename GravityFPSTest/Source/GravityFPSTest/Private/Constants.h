// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class GRAVITYFPSTEST_API Constants
{
public:
	// Player Constants
	inline static const float c_GravityScale = 1.75f;

	inline static const float c_SpawnOffset = 200.0f;
	inline static const float c_ArmourBaseWalkingSpeed = 600.0f;
	inline static const float c_ArmourBaseJumpHeight = 420.0f;

	inline static const float c_HumanBaseWalkingSpeed = 600.0f;
	inline static const float c_HumanMaxWalkingSpeed = 1200.0f;
	inline static const float c_HumanBaseJumpHeight = 420.0f;
	inline static const float c_HumanMaxJumpHeight = 720.0f;

	inline static const float c_HorizontalPush = 600.0f;
	inline static const float c_WallGravity = 0.3f;

	// Flying Constants
	inline static const float c_ThrustAccel = 3000.0f;
	inline static const float c_MaxFlyingSpeed = 4000.0f;
	inline static const float c_MaxFuelCapacity = 4000.0f;
	inline static const float c_FuelCost = 1.0f;
	inline static const float c_MaxFuelOptimizationLevel = 1.0f;

	// Missile Constants
	inline static const float c_MissileSpawnDelay = 0.05f;

	// Tank Rifle Constants
	inline static const float c_NukeChargeRate = 10.0f;
	inline static const float c_NukeMaxCharge = 30000.0f;
	inline static const float c_DestructibleDamage = 100.0f;

	// Invisibility Constants
	inline static const float c_InvisibilityDuration = 180.0f;

	// Radar Constants
	inline static const float c_RadarRadius = 1000.0f;

	// Biopad Constants
	inline static const float c_TraceRange = 200.0f;

	// UI icon Size Constants
	inline static const float c_IconSize = 64.0f;

	// scan constants

	inline static const float c_OffsetDistance = 100.0f;
	inline static const float c_DoorDetectionRange = 1000.0f;
};
