// Copyright © 2021 Tanzq. All rights reserved.


#include "ALS_Character.h"


// Add default functionality here for any IALS_Character_API functions that are not pure virtual.

void IALS_Character::API_GetCurrentStates_Implementation(EALS_MovementState& m_MovementState, EALS_MovementAction& m_MovementAction, EALS_Gait& m_Gait,
	EALS_Stance& m_Stance, EALS_RotationMode& RotationMode)
{
}

void IALS_Character::API_GetEssentialValues_Implementation(FVector& Velocity, FVector& m_Acceleration,
	bool& IsMoving, bool& HasMovementInput, float& fSpeed, FRotator& AimingRotation, float& m_AimYawRate)
{
}

void IALS_Character::API_SetMovementAction_Implementation(EALS_MovementAction m_MovementAction)
{
}
