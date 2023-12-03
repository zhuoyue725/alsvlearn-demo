// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TurnInPlace_Asset.generated.h"


USTRUCT(BlueprintType)
struct FTurnInPlace_Asset
{
	GENERATED_BODY()

	FTurnInPlace_Asset(): Animation(), AnimatedAngle(0), PlayRate(0), bScaleTurnAngle(false)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* Animation;

	
	/**
	 * @brief 每一帧旋转的角度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimatedAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bScaleTurnAngle;
};
