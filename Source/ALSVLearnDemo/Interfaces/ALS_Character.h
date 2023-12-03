// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALSVLearnDemo/Data/Enums/ALS_Gait.h"
#include "ALSVLearnDemo/Data/Enums/ALS_MovementAction.h"
#include "ALSVLearnDemo/Data/Enums/ALS_Stance.h"
#include "ALSVLearnDemo/Data/Structs/ALS_MovementState.h"
#include "ALS_Character.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UALS_Character : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ALSVLEARNDEMO_API IALS_Character
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	///////////////////////////////////////////////////
	/// character Information

	/**
	* @brief 获取记录角色状态的变量
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Character Information")
	void API_GetCurrentStates(EALS_MovementState& m_MovementState, EALS_MovementAction& m_MovementAction, EALS_Gait& m_Gait, EALS_Stance& m_Stance, EALS_RotationMode& Eals_RotationMode) ;
	
	virtual void API_GetCurrentStates_Implementation(EALS_MovementState& m_MovementState, EALS_MovementAction& m_MovementAction, EALS_Gait& m_Gait, EALS_Stance& m_Stance, EALS_RotationMode& Eals_RotationMode) ;
	
	/**
	* @brief 获取角色与输入有关的变量
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Character Information")
	void API_GetEssentialValues(FVector& Velocity, FVector& m_Acceleration, bool& IsMoving, bool& HasMovementInput, float& fSpeed, FRotator& AimingRotation, float& m_AimYawRate);
	
	virtual void API_GetEssentialValues_Implementation(FVector& Velocity, FVector& m_Acceleration, bool& IsMoving, bool& HasMovementInput, float& fSpeed, FRotator& AimingRotation, float& m_AimYawRate);

	///////////////////////////////////////////////////
	/// character States

	/**
	* @brief 设置 MovementAction 状态
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Character Information")
	void API_SetMovementAction(EALS_MovementAction m_MovementAction);

	virtual void API_SetMovementAction_Implementation(EALS_MovementAction m_MovementAction);
};
