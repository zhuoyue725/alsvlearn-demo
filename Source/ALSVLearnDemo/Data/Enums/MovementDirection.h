// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementDirection.generated.h"

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	EMD_Forward UMETA(DisplayName = "Forward"),
	EMD_Right UMETA(DisplayName = "Right"),
	EMD_Left UMETA(DisplayName = "Left"),
	EMD_Backward UMETA(DisplayName = "Backward"),
};
