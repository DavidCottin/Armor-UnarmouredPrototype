// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ClosestActorUtils.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITYFPSTEST_API UClosestActorUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static AActor* FindClosestRelevantActor(UWorld* World, AActor* ReferenceActor, const TArray<AActor*>& ActorList, const TArray<AActor*>& IgnoreActors, bool IncludeStaticMesh = false);

	// Unreal gets real fussy if you try to initialize a TArray inside a function declaration, so the overloaded function calls the one above but passes in a blank TArray
	static AActor* FindClosestRelevantActor(UWorld* World, AActor* ReferenceActor, const TArray<AActor*>& ActorList, bool IncludeStaticMesh = false);
	
};
