// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ALS_MovementAction.generated.h"

UENUM(BlueprintType)
enum class EALS_MovementAction : uint8
{
	/**
	 * @brief 无状态
	 */
	EAMs_None UMETA(DisplayName = "None"),
	
	/**
	 * @brief 攀爬地物体
	 */
	EAMs_LowMantle UMETA(DisplayName = "LowMantle"),
	
	/**
	 * @brief 攀爬高物体
	 */
	EAMs_HighMantle UMETA(DisplayName = "HighMantle"),
	
	/**
	 * @brief 翻滚状态
	 */
	EAMs_Rolling UMETA(DisplayName = "Rolling"),
	
	/**
	 * @brief 起身状态
	 */
	EAMs_GettingUp UMETA(DisplayName = "GettingUp"),
};
