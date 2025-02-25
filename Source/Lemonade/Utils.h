#pragma once

#include "CoreMinimal.h"

FORCEINLINE float InverseLerpClamped(float A, float B, float X)
{
    return FMath::Clamp((X - A) / (B - A), 0.0f, 1.0f);
}
