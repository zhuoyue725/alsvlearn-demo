// Fill out your copyright notice in the Description page of Project Settings.


#include "ALS_PlayerCameraBehavior.h"

void UALS_PlayerCameraBehavior::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UALS_PlayerCameraBehavior::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	 
}

APlayerController* UALS_PlayerCameraBehavior::GetPlayerController() const
{
	return PlayerController;
}

void UALS_PlayerCameraBehavior::SetPlayerController(APlayerController* const m_PlayerController)
{
	PlayerController = m_PlayerController;
}

APawn* UALS_PlayerCameraBehavior::GetControlledPawn() const
{
	return ControlledPawn;
}

void UALS_PlayerCameraBehavior::SetControlledPawn(APawn* const m_ControlledPawn)
{
	ControlledPawn = m_ControlledPawn;
}
