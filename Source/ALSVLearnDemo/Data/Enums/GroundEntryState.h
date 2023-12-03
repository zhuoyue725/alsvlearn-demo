// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GroundEntryState.generated.h"

UENUM(BlueprintType)
enum class EGroundedEntryState : uint8
{
	EGEs_None UMETA(DisplayName = "None"),
	EGEs_Roll UMETA(DisplayName = "Roll"),
};
