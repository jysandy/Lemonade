#pragma once

#include "CoreMinimal.h"

FORCEINLINE float InverseLerpClamped(float A, float B, float X)
{
    return FMath::Clamp((X - A) / (B - A), 0.0f, 1.0f);
}

FORCEINLINE FVector SlerpAndScaleVector(FVector Start, FVector End, float Alpha)
{
    FVector StartUnit;
    float StartLength;

    Start.ToDirectionAndLength(StartUnit, StartLength);

    FVector Out = FVector::SlerpVectorToDirection(StartUnit,
        End, 
        Alpha);

    Out *= FMath::Lerp(StartLength, End.Length(), Alpha);

    return Out;
}
