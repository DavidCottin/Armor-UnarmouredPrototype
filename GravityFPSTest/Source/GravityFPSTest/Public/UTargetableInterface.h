#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UTargetableInterface.generated.h"

// The UInterface class that Unreal uses for reflection and metadata
UINTERFACE(Blueprintable, BlueprintType)
class GRAVITYFPSTEST_API UTargetableInterface : public UInterface
{
    GENERATED_BODY()
};

// The IInterface class where we don't need any methods, it's just a tagging interface
class GRAVITYFPSTEST_API ITargetableInterface
{
    GENERATED_BODY()

public:
    ITargetableInterface();
    bool HaveIMoved() {return bIHaveMoved;};
    void MoveHasBeenProcessed() { bIHaveMoved = false; };

protected:
    bool bIHaveMoved;

private:
};