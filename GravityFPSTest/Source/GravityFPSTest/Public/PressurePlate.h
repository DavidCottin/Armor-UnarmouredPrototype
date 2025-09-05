// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressurePlate.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlateEvent);
UCLASS()
class GRAVITYFPSTEST_API APressurePlate : public AActor
{
	GENERATED_BODY()
	
    public:
        APressurePlate();

    protected:
        UPROPERTY(VisibleAnywhere)
        UBoxComponent* TriggerVolume;

        virtual void BeginPlay() override;

    public:
        UPROPERTY(BlueprintAssignable, Category = "Pressure Plate")
        FOnPlateEvent OnPlateActivated;

        UPROPERTY(BlueprintAssignable, Category = "Pressure Plate")
        FOnPlateEvent OnPlateDeactivated;

        UPROPERTY(EditAnywhere)
        UStaticMeshComponent* StaticMeshComponent;

    private:
        UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);

        UFUNCTION()
        void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    };