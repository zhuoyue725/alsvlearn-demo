// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ALS_MovementState.generated.h"

UENUM(BlueprintType)
enum class EALS_MovementState : uint8
{
	EAMs_None UMETA(DisplayName = "None"),
	EAMs_Grounded UMETA(DisplayName = "Grounded"),
	EAMs_InAir UMETA(DisplayName = "InAir"),
	EAMs_Mantling UMETA(DisplayName = "Mantling"),
	EAMs_Ragdoll UMETA(DisplayName = "Ragdoll"),
};
