// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ALS_ViewMode.generated.h"

UENUM(BlueprintType)
enum class EALS_ViewMode : uint8
{
	/**
	 * @brief 第三人称模式
	 */
	EAVm_ThirdPerson UMETA(DisplayName = "ThirdPerson"),
	
	/**
	 * @brief 第一人称模式
	 */
	EAVm_FirstPerson UMETA(DisplayName = "FirstPerson"),
};
