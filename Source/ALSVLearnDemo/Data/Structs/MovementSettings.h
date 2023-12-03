// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "Engine/DataTable.h"
#include "MovementSettings.generated.h"

USTRUCT(BlueprintType)
struct FMovementSettings
{
	GENERATED_BODY()
	
	/**
	 * @brief 行走速度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed;

	/**
	 * @brief 跑步速度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed;

	/**
	 * @brief 冲刺速度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SprintSpeed;

	/**
	 * @brief 运动曲线
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveVector* MovementCurve;

	/**
	 * @brief 旋转曲线
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationRateCurve;
};

USTRUCT(BlueprintType)
struct FMovementSettingsStance
{
	GENERATED_BODY()
	
	/**
	 * @brief 站立状态的运动数据
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettings Standing;

	/**
	 * @brief 蹲伏状态的运动数据
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettings Crouching;
	
};


USTRUCT(BlueprintType)
struct FMovementSettingsState  : public FTableRowBase
{
	GENERATED_BODY()
	/**
	* @brief 运动状态下的速度
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettingsStance VelocityDirection;

	
	/**
	* @brief 观察状态下的数据
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettingsStance LookingDirection;

	
	/**
	* @brief 瞄准状态下的数据
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettingsStance Aiming;
};
