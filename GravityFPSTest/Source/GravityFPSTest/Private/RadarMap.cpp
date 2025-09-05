// Fill out your copyright notice in the Description page of Project Settings.


#include "RadarMap.h"
#include "Kismet/GameplayStatics.h"
#include "BlipUserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Constants.h"
#include "GravityFPSTest/GravityFPSTestCharacter.h"

void URadarMap::NativeConstruct()
{
    Super::NativeConstruct();
    DetectionRange = 1000.0f;
    bInitialized = false;
    bMustUpdateRadarSize = true;

    // call EventUpdateDetection every one fourth of a second
    GetWorld()->GetTimerManager().SetTimer(OneSecondTimerHandle, this, &URadarMap::EventUpdateDetection, 0.2f, true);
    GetWorld()->GetTimerManager().SetTimer(NativeTickHandle, this, &URadarMap::FakeNativeTick, 0.1f, true);
}

void URadarMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    
}

void URadarMap::EventUpdateDetection()
{
    TArray<FHitResult> HitResults;
    AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* Pawn = pController->GetPawn();
    FVector Start = Pawn->GetActorLocation();
    FVector End = Start + FVector(0.0f, 0.0f, DetectionRange);
    float Radius = DetectionRange; // Sphere radius

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Pawn); // We ignore ourself so that we don't appear as a red dot on our own radar.

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,       // Collision channel
     // FCollisionShape::MakeCapsule(Radius, 18000),
        FCollisionShape::MakeSphere(Radius),
        QueryParams
    );

    // old actors should not persist
    EnemyActors.Empty();

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (!HitActor) continue;

            if (HitActor->Tags.Contains(FName("HomingTarget")) || HitActor->GetClass()->ImplementsInterface(UTargetableInterface::StaticClass()))
            {
                EnemyActors.AddUnique(HitActor);
        //        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Blocked by: %s"), *Hit.GetActor()->GetName()));
            }
#if 0
            EnemyActors.AddUnique(Pawn); // Activate for debugging Radar offset.
#endif
        }
    }
}

void URadarMap::FakeNativeTick()
{
    if (!bInitialized)
    {
        RadarSize = GetCachedGeometry().GetLocalSize();

        if (RadarSize.X > 0 && RadarSize.Y > 0) // ensure layout is valid
        {
            WidgetRadiusInPixels = RadarSize / 2.0f;
            check(WidgetRadiusInPixels.X == WidgetRadiusInPixels.Y); // if needed
            WidgetScaleFactor = DetectionRange / WidgetRadiusInPixels.X;

            bInitialized = true;
        }
    }

    CP_Blips->ClearChildren();
    AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* Pawn = pController->GetPawn();
    FVector PlayerLocation = Pawn->GetActorLocation();
    FRotator PlayerRotation = Pawn->GetActorRotation();
    PlayerRotation.Pitch = 0;
    PlayerRotation.Roll = 0;
    PlayerRotation.Yaw += 90.0f; // add rotation offset here

    FRotator RadarRotation = FRotator(0.0f, -PlayerRotation.Yaw, 0.0f);

    for (AActor* Actor : EnemyActors)
    {
        FVector EnemyLocation = Actor->GetActorLocation();
        FVector DisplacementFromPlayer = EnemyLocation - PlayerLocation;

        FRotator InverseRotation = FRotator(0.0f, -PlayerRotation.Yaw, 0.0f);
        FVector Displacement2 = InverseRotation.RotateVector(DisplacementFromPlayer);
        DisplacementFromPlayer = Displacement2;

        // we do not care about our z value here.
        DisplacementFromPlayer.Z = 0.0f;

        //   GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Displacement = %f, %f"), DisplacementFromPlayer.X, DisplacementFromPlayer.Y));
           // we divide by detection range here so that the object will appear within our radar scope.
        float Distance = DisplacementFromPlayer.Length();

        //   GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Distance = %f"), Distance));

        Distance /= WidgetScaleFactor;
        DisplacementFromPlayer.Normalize();
        DisplacementFromPlayer *= Distance;

        if (BlipWidgetClass)
        {
            if (UBlipUserWidget* BlipWidget = CreateWidget<UBlipUserWidget>(GetWorld(), BlipWidgetClass))
            {
                if (Distance < WidgetRadiusInPixels.X)
                {
                    CP_Blips->AddChild(BlipWidget);
                    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(BlipWidget->Slot);

                    if (CanvasSlot)
                    {
                        FVector2D FinalPosition = FVector2D(DisplacementFromPlayer.X, DisplacementFromPlayer.Y) + WidgetRadiusInPixels;

                        //               if (Actor->Tags.Contains(FName("ObjectOfInterest")))
                        {
#if 0
                            UE_LOG(LogTemp, Warning, TEXT("RadarSize = %9.5f, %9.5f / Player Pos = %6.2f, %6.2f / Enemy %-20s Pos = %6.2f, %6.2f / Disp = %6.2f, %6.2f / on Radar = %6.2f, %6.2f"),
                                RadarSize.X, RadarSize.Y, PlayerLocation.X, PlayerLocation.Y, *Actor->GetName(), EnemyLocation.X, EnemyLocation.Y,
                                DisplacementFromPlayer.X, DisplacementFromPlayer.Y,
                                FinalPosition.X, FinalPosition.Y);
#endif
                        }
                        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f)); // Ensure pivot is centered
                        CanvasSlot->SetPosition(FinalPosition);
                    }
                }
            }
        }
    }
}
