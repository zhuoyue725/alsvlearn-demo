// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LeanAmount.generated.h"

USTRUCT(BlueprintType)
struct FLeanAmount
{
	GENERATED_BODY()

	FLeanAmount(float LR = 0, float FB = 0): LR(LR), FB(FB)
	{
	}

	// 左右边的倾斜值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LR;

	// 前后的倾斜值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FB;
};
