// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ALS_RotationMode.generated.h"

UENUM(BlueprintType)
enum class EALS_RotationMode : uint8
{
	EARm_VelocityDirection UMETA(DisplayName = "VelocityDirection"),
	
	EARm_LookingDirection UMETA(DisplayName = "LookingDirection"),
	
	EARm_Aiming UMETA(DisplayName = "Aiming"),
};
