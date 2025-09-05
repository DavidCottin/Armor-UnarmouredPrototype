// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TankRifleProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class USoundBase;
class URadialForceComponent;

UCLASS()
class GRAVITYFPSTEST_API ATankRifleProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATankRifleProjectile();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particle visuals")
	UParticleSystemComponent* ExplosionParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
	UParticleSystem* ExplosionParticleSystem;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly)
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* ImpactSound;

	UFUNCTION()
	void PlayFireSound();

	UFUNCTION()
	void PlayImpactSound();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float LifeTime;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddVelocity(FVector Velocity);
private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
