// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EngineUtils.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"
#include "Constants.h"
#include "GravityFPSTest/GravityFPSTestCharacter.h"
#include "UTargetableInterface.h"
#include "ClosestActorUtils.h"

// Sets default values
ACubeProjectile::ACubeProjectile() : LifeTime(0.1f), bActivated(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
    CollisionComponent->SetRelativeScale3D(FVector(0.2f));
    CollisionComponent->SetNotifyRigidBodyCollision(true);	// set up a notification for when this component hits something blocking
    CollisionComponent->BodyInstance.SetCollisionProfileName("BlockAll");
    CollisionComponent->OnComponentHit.AddDynamic(this, &ACubeProjectile::OnHit);

    // Set as root component
    RootComponent = CollisionComponent;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComponent;
    ProjectileMovement->InitialSpeed = 10.0f;
    ProjectileMovement->MaxSpeed = 0.0f; // 0 means no limit for max speed
    ProjectileMovement->ProjectileGravityScale = Constants::c_GravityScale;
    ProjectileMovement->bRotationFollowsVelocity = false;
    ProjectileMovement->bShouldBounce = false;

    StaticMeshComponent->SetupAttachment(RootComponent);

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

    bReplicates = true;
    SetReplicateMovement(true);
    SetActorTickEnabled(false);

}

void ACubeProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    FVector Location = GetActorLocation();
    UE_LOG(LogTemp, Warning, TEXT("CubeProjectile destroyed at location: %s"), *Location.ToString());
}

void ACubeProjectile::PlayFireSound()
{
    UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
}

void ACubeProjectile::PlayImpactSound()
{
    UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
}

// Called when the game starts or when spawned
void ACubeProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ACubeProjectile::AddVelocity(FVector Velocity)
{
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity += Velocity;
    }
}

// Called every frame
void ACubeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (bActivated)
    {
        LifeTime -= DeltaTime;
        if (LifeTime <= 0.0f)
        {
            ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
            if (PlayerCharacter)
            {
                AGravityFPSTestCharacter* Player = Cast<AGravityFPSTestCharacter>(PlayerCharacter);
                if (Player)
                {
                    TArray<AActor*> FoundActors;
                    UWorld* world = GetWorld();
                    UGameplayStatics::GetAllActorsWithInterface(world, UTargetableInterface::StaticClass(), FoundActors);
                    // To test with Static Mesh Actors:
#if 1
                    TArray<AActor*> MeshActors;
                    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), MeshActors);
                    FoundActors.Append(MeshActors);
#endif
                    AActor* Closest = UClosestActorUtils::FindClosestRelevantActor(world, this, FoundActors, true);
                    if (Closest)
                    {
                        Closest->SetActorLocation(Player->GetSavedLocation());
                    }
                    PlayImpactSound();
                    if (ExplosionParticleSystem)
                    {
                        UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(
                            GetWorld(),
                            ExplosionParticleSystem,
                            GetActorTransform());
                    }
                    Destroy();
                }
            }
        }
    }
}

void ACubeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if ((OtherActor != nullptr) && (OtherActor != this) && (!OtherActor->IsA(ACubeProjectile::StaticClass())) && (OtherComp != nullptr))
    {
        if (OtherComp->GetOwner()) // GetOwner will return true if it's connected to an actor and false if it's not.
        {
            bActivated = true;
            PlayFireSound();
        }
    }
    else
    {
        Destroy();
    }
}

void ACubeProjectile::Hold(USkeletalMeshComponent* HoldingComponent)
{
    if (HoldingComponent)
    {
        //DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 300, FColor::Yellow, false, 1, 0, 5.f);
        SetActorTickEnabled(false);
        CollisionComponent->SetSimulatePhysics(false);
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CollisionComponent->SetGenerateOverlapEvents(false);
        CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
        SetActorEnableCollision(false);
        if (ProjectileMovement)
        {
            ProjectileMovement->StopMovementImmediately();
            ProjectileMovement->Deactivate(); // Disable further movement updates
        }
        AttachToComponent(HoldingComponent,
            FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
            TEXT("HeldItemSocket"));
        SetActorRelativeLocation(FVector::ZeroVector);
        SetActorRelativeRotation(FRotator::ZeroRotator);
        // Calculate offset so mesh matches the socket
 //       FTransform SocketTransform = HoldingComponent->GetSocketTransform(TEXT("GripPoint"), RTS_World);
 //       FTransform MeshTransform = StaticMeshComponent->GetComponentTransform();
 //       FVector Offset = SocketTransform.GetLocation() - MeshTransform.GetLocation();
 //       UE_LOG(LogTemp, Warning, TEXT("Socket World Transform: %s"), *HoldingComponent->GetSocketTransform(TEXT("GripPoint"), RTS_World).ToString());
 //       UE_LOG(LogTemp, Warning, TEXT("Cube World Transform: %s"), *GetActorTransform().ToString());

 //       AddActorWorldOffset(Offset);
        
    }
}

void ACubeProjectile::Throw(FVector Direction)
{
    SetActorTickEnabled(true);
    CollisionComponent->SetSimulatePhysics(true);
    CollisionComponent->SetNotifyRigidBodyCollision(true);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
    SetActorEnableCollision(true);
    CollisionComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    if (ProjectileMovement)
    {
        ProjectileMovement->Activate(); // Re-enable movement
        ProjectileMovement->Velocity = Direction * 1000.0f; // TODO: Make this value a constant.
    }
}
