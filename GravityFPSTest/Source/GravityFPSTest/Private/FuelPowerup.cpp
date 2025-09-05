// Fill out your copyright notice in the Description page of Project Settings.


#include "FuelPowerup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FlyingTimerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GravityFPSTest/GravityFPSTestCharacter.h"

// Sets default values
AFuelPowerup::AFuelPowerup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerupMesh"));

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(60.0f);
  //  CollisionComp->SetNotifyRigidBodyCollision(true);	// set up a notification for when this component hits something blocking
    CollisionComp->BodyInstance.SetCollisionProfileName("OverlapAll");
    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AFuelPowerup::OnHit);

    // Set as root component
    RootComponent = CollisionComp;
    StaticMeshComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AFuelPowerup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFuelPowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    FRotator RotationDelta(0.0f, 90.0f * DeltaTime, 0.0f);
    AddActorLocalRotation(RotationDelta);

}

void AFuelPowerup::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
    {
        if (OtherComp->GetOwner() && OtherActor->IsA(AGravityFPSTestCharacter::StaticClass())) // GetOwner will return true if it's connected to an actor and false if it's not.
        {
            AGravityFPSTestCharacter* Character = Cast<AGravityFPSTestCharacter>(OtherActor);
            if (Character)
            {
                Character->GetFuelComponent()->ResetMaxFuel();
                UGameplayStatics::PlaySoundAtLocation(this, CollectionSoundEffect, GetActorLocation());
                Destroy();
            }
        }
    }
}

