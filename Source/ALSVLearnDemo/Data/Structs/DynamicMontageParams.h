// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMontageParams.generated.h"

USTRUCT(BlueprintType)
struct FDynamicMontageParams
{
	GENERATED_BODY()

	FDynamicMontageParams(): Animation(nullptr), BlendInTime(0), BlendOutTime(0), PlayRate(0), StartTime(0)
	{
	}

	/**
	 * @brief 动画序列
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* Animation;

	/**
	 * @brief 淡入时间
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlendInTime;

	/**
	 * @brief 淡出时间
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlendOutTime;

	/**
	 * @brief 播放速率
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayRate;

	/**
	 * @brief 开始时间
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTime;
};
