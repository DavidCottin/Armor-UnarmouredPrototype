// Fill out your copyright notice in the Description page of Project Settings.


#include "TankRifleProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EngineUtils.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Constants.h"
#include "UTargetableInterface.h"
#include "ClosestActorUtils.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "GravityFPSTest/GravityFPSTestCharacter.h"

// Sets default values
ATankRifleProjectile::ATankRifleProjectile() : LifeTime(10.0f)
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");


    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(60.0f);
    CollisionComp->SetNotifyRigidBodyCollision(true);	// set up a notification for when this component hits something blocking
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->OnComponentHit.AddDynamic(this, &ATankRifleProjectile::OnHit);

    // Set as root component
    RootComponent = CollisionComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 0.0f;
    ProjectileMovement->MaxSpeed = 0.0f; // 0 means no limit for max speed.
    ProjectileMovement->ProjectileGravityScale = Constants::c_GravityScale;
    ProjectileMovement->bRotationFollowsVelocity = false;
    ProjectileMovement->bShouldBounce = false;

    RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
    RadialForceComponent->SetupAttachment(RootComponent);
    RadialForceComponent->Radius = 1000.0f;
    RadialForceComponent->ForceStrength = 9250;
    RadialForceComponent->ImpulseStrength = 4000;

    RadialForceComponent->bImpulseVelChange = true;
    RadialForceComponent->bAutoActivate = false;
    RadialForceComponent->bIgnoreOwningActor = true;

    StaticMeshComponent->SetupAttachment(RootComponent);
    StaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

    // Particle effect setup (visuals)
    ExplosionParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Explosion Particles"));
    ExplosionParticleComponent->SetupAttachment(RootComponent);
    // Load the particle effect asset (use Unreal default explosion effect until custom asset is made)
    static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT(
        "ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
    if (ParticleAsset.Succeeded())
    {
        ExplosionParticleSystem = ParticleAsset.Object;
        // Update the particle component template so that it is visible in the actor.
        ExplosionParticleComponent->SetTemplate(ExplosionParticleSystem);
    }
}

void ATankRifleProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    bool playSound = true;
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
    {
        if (OtherComp->GetOwner()) // GetOwner will return true if it's connected to an actor and false if it's not.
        {
            PlayImpactSound();
            // destroy the actor only if it does not have the Indestructible tag.
            if (!OtherActor->Tags.Contains(FName("Indestructible")))
            {
                OtherComp->DestroyComponent();
            }
            playSound = false;
        }
        RadialForceComponent->DestructibleDamage = Constants::c_DestructibleDamage;
        RadialForceComponent->bIgnoreOwningActor = false;
        RadialForceComponent->Activate();
        RadialForceComponent->FireImpulse();
        Destroy();
    }

    // Check if a particle system is assigned in the parent class, and spawn it at the actor's location.
    if (ExplosionParticleSystem)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticleSystem, GetActorLocation());
    }
    if (playSound)
    {
        PlayImpactSound();
    }
    Destroy();
}

void ATankRifleProjectile::PlayFireSound()
{
    UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
}

void ATankRifleProjectile::PlayImpactSound()
{
    UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
}

// Called when the game starts or when spawned
void ATankRifleProjectile::BeginPlay()
{
    Super::BeginPlay();
    PlayFireSound();
}

// Called every frame
void ATankRifleProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    LifeTime -= DeltaTime;
    if (LifeTime <= 0.0f)
    {
        Destroy();
    }
}

/// <summary>Adds the velocity to the initial velocity of the object, useful for when spawning an object from another object already in motion.</summary>
/// <param name="velocity">Takes an FVector as a parameter and adds it to the object's initial velocity.</param>
/// <returns>return type is void</returns>
void ATankRifleProjectile::AddVelocity(FVector Velocity)
{
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity += Velocity;
    }
}
