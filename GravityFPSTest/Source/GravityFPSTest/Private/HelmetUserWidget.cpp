
#include "HelmetUserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

bool UHelmetUserWidget::Initialize()
{
	bool bResult = Super::Initialize();
	pct = 0.0f;
	helmetState = EHelmetState::Hidden;
	UWidget* HelmetImage = GetWidgetFromName(TEXT("HelmetImage"));
	UWidget* HelmetScreen = GetWidgetFromName(TEXT("HelmetImageOpacity"));
	if (HelmetImage)
	{
		HelmetImage->SetRenderOpacity(0.0f);
	}
	if (HelmetScreen)
	{
		HelmetScreen->SetRenderOpacity(0.0f);
	}

	if (!bResult)
	{
		return false;
	}
	return true;
}

void UHelmetUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (helmetState == EHelmetState::FadingIn || helmetState == EHelmetState::FadingOut)
	{
		if (helmetState == EHelmetState::FadingIn)
		{
			pct += InDeltaTime;
		}
		else if (helmetState == EHelmetState::FadingOut)
		{
			pct -= InDeltaTime;
		}
		float LerpedValue = FMath::Lerp(0.0f, 1.0f, pct);
		UWidget* HelmetImage = GetWidgetFromName(TEXT("HelmetImage"));
		UWidget* HelmetScreen = GetWidgetFromName(TEXT("HelmetImageOpacity"));
		if (HelmetImage)
		{
			HelmetImage->SetRenderOpacity(LerpedValue);
		}
		if (HelmetScreen)
		{
			HelmetScreen->SetRenderOpacity(LerpedValue/4.0f);
		}
		if (pct >= 1.0f)
		{
			helmetState = EHelmetState::Visible;
		}
		else if (pct <= 0.0f)
		{
			helmetState = EHelmetState::Hidden;
		}
	}
}

void UHelmetUserWidget::PlayFadeIn()
{
	helmetState = EHelmetState::FadingIn;
}

void UHelmetUserWidget::PlayFadeOut()
{
	helmetState = EHelmetState::FadingOut;
}