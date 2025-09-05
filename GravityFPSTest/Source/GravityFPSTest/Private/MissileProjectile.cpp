// Fill out your copyright notice in the Description page of Project Settings.

#include "MissileProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EngineUtils.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Constants.h"
#include "UTargetableInterface.h"
#include "ClosestActorUtils.h"
#include "GravityFPSTest/GravityFPSTestCharacter.h"
#include "MissileManager.h"

// Sets default values
AMissileProjectile::AMissileProjectile() : LifeTime(10.0f)
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");


    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(60.0f);
    CollisionComp->SetNotifyRigidBodyCollision(true);	// set up a notification for when this component hits something blocking
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->OnComponentHit.AddDynamic(this, &AMissileProjectile::OnHit);

    // Set as root component
    RootComponent = CollisionComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.0f;// 0 means no limit for max speed.
    ProjectileMovement->ProjectileGravityScale = 0.0f;
    ProjectileMovement->bRotationFollowsVelocity = false;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->bIsHomingProjectile = true;
    ProjectileMovement->HomingAccelerationMagnitude = 2000000.f;

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

void AMissileProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    bool playSound = true;
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
    {
        if (OtherComp->GetOwner()) // GetOwner will return true if it's connected to an actor and false if it's not.
        {
            PlayImpactSound();
            OtherComp->AddImpulseAtLocation(GetVelocity() * 10, GetActorLocation());
            playSound = false;
        }
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

void AMissileProjectile::PlayFireSound()
{
     UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
}

void AMissileProjectile::PlayImpactSound()
{
     UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
}

// Called when the game starts or when spawned
void AMissileProjectile::BeginPlay()
{
    Super::BeginPlay();
    PlayFireSound();

    // this may need to be reworked for a networked game, unsure. But it works perfectly fine for a local one.
    AGravityFPSTestCharacter* Player = Cast<AGravityFPSTestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    TArray<AActor*> FoundActors;

    UWorld* World = GetWorld();
    TArray<AActor*> MeshActors;
    MeshActors = Player->GetActorsInConeFromCamera(3600.0f, 15000000000, 45.0f);

    for (AActor* Actor : MeshActors)
    {
        if (Actor->Tags.Contains(FName("HomingTarget")) || Actor->GetClass()->ImplementsInterface(UTargetableInterface::StaticClass()))
        {
            FoundActors.Add(Actor);
        }
    }

    TArray<AActor*> VisibleActors;
    for (AActor* Actor : FoundActors)
    {
        if (Actor->WasRecentlyRendered(0.01f))
        {
            VisibleActors.Add(Actor);
        }
    }

    AActor* Closest = UClosestActorUtils::FindClosestRelevantActor(World, this, VisibleActors, true);
    if (Closest)
    {
        ProjectileMovement->bIsHomingProjectile = true;
        ProjectileMovement->HomingTargetComponent = Closest->GetRootComponent();
    }
    UMissileManagerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMissileManagerSubsystem>();
    Subsystem->ActiveMissiles.Add(this);
}

void AMissileProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    UMissileManagerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMissileManagerSubsystem>();
    Subsystem->ActiveMissiles.Remove(this);
}
// Called every frame
void AMissileProjectile::Tick(float DeltaTime)
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
void AMissileProjectile::AddVelocity(FVector Velocity)
{
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity += Velocity;
    }
}