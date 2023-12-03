// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSVLearnDemoCharacter.h"
#include "ALS_AnimMan_Character.generated.h"

/**
 * 
 */
UCLASS()
class ALSVLEARNDEMO_API AALS_AnimMan_Character : public AALSVLearnDemoCharacter
{
	GENERATED_BODY()

public:


	
	//////////////////////////////////////////////////////////////////////////
	/// 重载函数

	virtual FTransform Get3PPivotTarget_Implementation() override;


	virtual UAnimMontage* GetRollAnimation() override;


	//////////////////////////////////////////////////////////////////////////
	/// Roll Animations

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Roll Animations")
	UAnimMontage* LandRollDefault;
	
};
