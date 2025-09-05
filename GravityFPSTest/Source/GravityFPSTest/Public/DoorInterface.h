// DoorInterface.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"   
#include "DoorInterface.generated.h"

UINTERFACE(Blueprintable)
class GRAVITYFPSTEST_API UDoorInterface : public UInterface
{
    GENERATED_BODY()
public:
};

class GRAVITYFPSTEST_API IDoorInterface
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door")
    void Interact();
};

