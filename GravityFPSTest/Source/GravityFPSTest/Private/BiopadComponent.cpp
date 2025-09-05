// Fill out your copyright notice in the Description page of Project Settings.


#include "BiopadComponent.h"
#include "Constants.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UBiopadComponent::UBiopadComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UBiopadComponent::TrySelect()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>();
    if (!Camera) return;

    FVector Start = Camera->GetComponentLocation();
    FVector End = Start + Camera->GetForwardVector() * Constants::c_TraceRange;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor && !SelectedActors.Contains(HitActor))
        {
            SelectedActors.AddUnique(HitActor); // AddUnique should not be needed here, but I'm trying to be defensive
        //    UE_LOG(LogTemp, Log, TEXT("Selected: %s"), *HitActor->GetName());
        }
    }

 //   DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 1.f);
}

void UBiopadComponent::RemoveLastSelected()
{
    if (SelectedActors.Num() > 0)
    {
        AActor* RemovedActor = SelectedActors[0];
        SelectedActors.RemoveAt(0);
        UE_LOG(LogTemp, Log, TEXT("Removed: %s"), *RemovedActor->GetName());
    }
}


// Called when the game starts
void UBiopadComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBiopadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    DisplayData.Empty();

    if (AActor* Owner = GetOwner())
    {
        FVector PlayerLocation = Owner->GetActorLocation();

        for (AActor* Actor : SelectedActors)
        {
            if (IsValid(Actor))
            {
                FVector Distance = Actor->GetActorLocation() - PlayerLocation;

                FActorInfoDisplay Info;
                Info.Name = Actor->GetName();
                Info.Distance = Distance;

                DisplayData.Add(Info);
            }
        }
    }
}

