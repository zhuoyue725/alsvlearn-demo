// Copyright © 2021 Tanzq. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ALSVLearnDemo/Data/Enums/GroundEntryState.h"
#include "UObject/Interface.h"
#include "ALS_Animation.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UALS_Animation : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ALSVLEARNDEMO_API IALS_Animation
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

	public:

	/**
	* @brief 实现跳跃相关
	*/
	UFUNCTION(BlueprintNativeEvent)
	void API_Jumped();

	virtual void API_Jumped_Implementation();
	
	/**
	* @brief 设置进入地面的状态变量
	*/
	UFUNCTION(BlueprintNativeEvent)
	void API_SetGroundedEntryState(EGroundedEntryState m_GroundedEntryState);

	virtual void API_SetGroundedEntryState_Implementation(EGroundedEntryState m_GroundedEntryState);
};
