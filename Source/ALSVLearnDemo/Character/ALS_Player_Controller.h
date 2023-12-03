// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ALS_Player_Controller.generated.h"

/**
 * 
 */
UCLASS()
class ALSVLEARNDEMO_API AALS_Player_Controller : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void OnPossess(APawn* InPawn) override;
};
