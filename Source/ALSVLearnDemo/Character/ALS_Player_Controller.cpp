// Fill out your copyright notice in the Description page of Project Settings.


#include "ALS_Player_Controller.h"

#include "ALSVLearnDemo/CameraSystem/ALS_CameraManager.h"

// 在控制器（Controller）类中定义的，它在控制器接管一个角色的控制权时被调用。当玩家或AI控制器接管角色时，该函数将执行特定的逻辑和行为
void AALS_Player_Controller::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AALS_CameraManager* CameraManager = Cast<AALS_CameraManager>(PlayerCameraManager);
	if (CameraManager)
	{
		CameraManager->OnPossess(InPawn);
	}
}
