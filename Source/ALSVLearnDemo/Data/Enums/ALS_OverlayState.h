// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ALS_OverlayState.generated.h"

UENUM(BlueprintType)
enum class EALS_OverlayState : uint8
{
	EAOs_Default UMETA(DisplayName = "Default"),
	EAOs_Masculine UMETA(DisplayName = "Masculine"),
	EAOs_Feminine UMETA(DisplayName = "Feminine"),
	EAOs_Injured UMETA(DisplayName = "Injured"),
	EAOs_HandsTied UMETA(DisplayName = "HandsTied"),
	EAOsRifle UMETA(DisplayName = "Rifle"),
	EAOs_Pistol_1H UMETA(DisplayName = "Pistol 1H"),
	EAOs_Pistol_2H UMETA(DisplayName = "Pistol 2H"),
	EAOs_Bow UMETA(DisplayName = "Bow"),
	EAOs_Torch UMETA(DisplayName = "Torch"),
	EAOs_Binoculars UMETA(DisplayName = "Binoculars"),
	EAOs_Box UMETA(DisplayName = "Box"),
	EAOs_Barrel UMETA(DisplayName = "Barrel"),
};