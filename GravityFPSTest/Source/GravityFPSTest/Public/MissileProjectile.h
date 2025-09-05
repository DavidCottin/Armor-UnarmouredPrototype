// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class USoundBase;

UCLASS()
class GRAVITYFPSTEST_API AMissileProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMissileProjectile();

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	float LifeTime;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddVelocity(FVector Velocity);
private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
