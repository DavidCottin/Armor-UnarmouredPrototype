// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BiopadComponent.generated.h"


USTRUCT(BlueprintType)
struct FActorInfoDisplay
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FVector Distance;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRAVITYFPSTEST_API UBiopadComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBiopadComponent();

	UFUNCTION(BlueprintCallable)
	void TrySelect();

	UFUNCTION(BlueprintCallable)
	void RemoveLastSelected();

	UFUNCTION(BlueprintCallable)
	const TArray<FActorInfoDisplay>& GetActorDisplayData() { return DisplayData; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> SelectedActors;

	UPROPERTY(BlueprintReadOnly)
	TArray<FActorInfoDisplay> DisplayData;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
