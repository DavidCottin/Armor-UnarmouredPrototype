// Fill out your copyright notice in the Description page of Project Settings.

#include "ClosestActorUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "ClosestActorUtils.h"

/// <summary>Finds the closest Actor to the provided reference location in the given world.</summary>
/// <param name="position, f">Takes a UWorld, Actor, and Actor subclass as parameters for the search, actor list and actors to ignore (if any) are also required.".</param>
/// <returns>return type is Actor</returns>
AActor* UClosestActorUtils::FindClosestRelevantActor(UWorld* World, AActor* ReferenceActor, const TArray<AActor*>& ActorList, const TArray<AActor*>& IgnoreActors, bool IncludeStaticMesh)
{
    if (!World || !ReferenceActor)
        return nullptr;

    AActor* Closest = nullptr;
    float ClosestDistSq = TNumericLimits<float>::Max();

    for (AActor* Actor : ActorList)
    {
        if (Actor == ReferenceActor || IgnoreActors.Contains(Actor))
        {
            continue;
        }

        if (!IncludeStaticMesh && Actor->IsA(AStaticMeshActor::StaticClass()))
        {
            continue;
        }

        float DistSq = FVector::DistSquared(Actor->GetActorLocation(), ReferenceActor->GetActorLocation());
        if (DistSq < ClosestDistSq)
        {
            ClosestDistSq = DistSq;
            Closest = Actor;
        }
    }

    return Closest;
}

AActor* UClosestActorUtils::FindClosestRelevantActor(UWorld* World, AActor* ReferenceActor, const TArray<AActor*>& ActorList, bool IncludeStaticMesh)
{
    TArray<AActor*> EmptyArray;
    return FindClosestRelevantActor(World, ReferenceActor, ActorList, EmptyArray, IncludeStaticMesh);
}
