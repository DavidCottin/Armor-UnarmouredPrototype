// Fill out your copyright notice in the Description page of Project Settings.


#include "BiopadUserWidget.h"
#include "BiopadComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Constants.h"
#include "Components/TextBlock.h"
#include "GravityFPSTest/GravityFPSTestCharacter.h"

bool UBiopadUserWidget::Initialize()
{
    bool bResult = Super::Initialize();
	if (!bResult)
	{
		return false;
	}
	DistanceInsteadOfCoordinates = false;
	return true;
}

void UBiopadUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* Pawn = pController->GetPawn();
	PlayerCharacter = Cast<AGravityFPSTestCharacter>(Pawn);
	if (PlayerCharacter)
	{
		UBiopadComponent* BiopadComponent = PlayerCharacter->GetBiopadComponent();
		TArray<FActorInfoDisplay> ActorsToDisplay = BiopadComponent->GetActorDisplayData();
		FString Result = "";
		for (const FActorInfoDisplay& Actor : ActorsToDisplay)
		{	
			if (DistanceInsteadOfCoordinates)
			{
				FString ActorInfo = FString::Printf(TEXT("%s | %s\n"),
					*Actor.Name,
					*FormatSignedInt(FMath::RoundToInt(Actor.Distance.Length())));
				Result += ActorInfo;
			}
			else
			{
				FString ActorInfo = FString::Printf(TEXT("%s | X:%s Y:%s Z:%s\n"),
					*Actor.Name,
					*FormatSignedInt(FMath::RoundToInt(Actor.Distance.X)),
					*FormatSignedInt(FMath::RoundToInt(Actor.Distance.Y)),
					*FormatSignedInt(FMath::RoundToInt(Actor.Distance.Z)));
				Result += ActorInfo;
			}
		}
		if (BiopadScreenText)
		{
			BiopadScreenText->SetText(FText::FromString(Result));
		}
	//	UE_LOG(LogTemp, Warning, TEXT("Selected: %s"), *Result);
	}
}

FString UBiopadUserWidget::FormatSignedInt(int32 Value)
{
	// This is because the negative symbol is treated as an extra character when parsing and I would like to avoid this
	if (Value < 0)
	{
		return FString::Printf(TEXT("-%05d"), FMath::Abs(Value));
	}
	else
	{
		return FString::Printf(TEXT(" %05d"), Value);
	}
	return FString();
}
