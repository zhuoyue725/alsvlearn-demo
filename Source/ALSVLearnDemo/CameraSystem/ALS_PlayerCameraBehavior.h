// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ALS_PlayerCameraBehavior.generated.h"

/**
 * 
 */
UCLASS()
class ALSVLEARNDEMO_API UALS_PlayerCameraBehavior : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	////////////////////////////////////////////////////////////////////////////////////
	/// 引用一些变量到该类中使用

	/**
	 * @brief 玩家控制器
	 */
	UPROPERTY(Category="References", VisibleAnywhere, BlueprintReadOnly)
	APlayerController* PlayerController;

	/**
	 * @brief 保存正在控制的Pawn类
	 */
	UPROPERTY(Category="References", VisibleAnywhere, BlueprintReadOnly)
	APawn* ControlledPawn;

public:
	
	APlayerController* GetPlayerController() const;
	void SetPlayerController(APlayerController* const PlayerController);
	
	APawn* GetControlledPawn() const;
	void SetControlledPawn(APawn* const m_ControlledPawn);
};
