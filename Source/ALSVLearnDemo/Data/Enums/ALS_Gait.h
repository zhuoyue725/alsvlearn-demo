// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ALS_Gait.generated.h"

UENUM(BlueprintType)
enum class EALS_Gait : uint8
{
	EAG_Walking UMETA(DisplayName = "Walking"),
	EAG_Running UMETA(DisplayName = "Running"),
	EAG_Sprinting UMETA(DisplayName = "Sprinting"),
};
