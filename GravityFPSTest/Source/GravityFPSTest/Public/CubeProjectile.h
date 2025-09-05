// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeProjectile.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class USoundBase;

UCLASS()
class GRAVITYFPSTEST_API ACubeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeProjectile();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particle visuals")
	UParticleSystemComponent* ExplosionParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
	UParticleSystem* ExplosionParticleSystem;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* FireSound;

	UFUNCTION()
	void PlayFireSound();

	UFUNCTION()
	void PlayImpactSound();

	void AddVelocity(FVector Velocity);
	UBoxComponent* GetCollisionComponent() { return CollisionComponent; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float LifeTime;
	bool bActivated;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void Hold(USkeletalMeshComponent* HoldingComponent);
	void Throw(FVector Direction);
};
