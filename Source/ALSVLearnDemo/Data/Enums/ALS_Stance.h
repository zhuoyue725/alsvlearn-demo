#pragma once

#include "CoreMinimal.h"
#include "ALS_Stance.generated.h"

UENUM(BlueprintType)
enum class EALS_Stance : uint8
{
	EAS_Standing UMETA(DisplayName = "Standing"),
	EAS_Crouching UMETA(DisplayName = "Crouching"),
};