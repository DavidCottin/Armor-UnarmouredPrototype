// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"
#include "Components/BoxComponent.h"

// Sets default values
APressurePlate::APressurePlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Register our Overlap Event
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PressurePlateMesh"));
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;
	StaticMeshComponent->SetupAttachment(RootComponent);

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::OnOverlapEnd);
}

void APressurePlate::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OnPlateActivated.IsBound())
	{
		OnPlateActivated.Broadcast();
	}
}

void APressurePlate::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OnPlateDeactivated.IsBound())
	{
		OnPlateDeactivated.Broadcast();
	}
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

