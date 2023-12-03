// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSVLearnDemo/Data/Structs/DynamicMontageParams.h"
#include "ALSVLearnDemo/Data/Structs/TurnInPlace_Asset.h"
#include "../Data/Enums/ALS_Gait.h"
#include "ALSVLearnDemo/Data/Enums/ALS_MovementAction.h"
#include "ALSVLearnDemo/Data/Enums/ALS_Stance.h"
#include "ALSVLearnDemo/Data/Enums/GroundEntryState.h"
#include "ALSVLearnDemo/Data/Enums/MovementDirection.h"
#include "ALSVLearnDemo/Data/Enums/ALS_RotationMode.h"
#include "ALSVLearnDemo/Data/Structs/ALS_MovementState.h"
#include "ALSVLearnDemo/Data/Structs/LeanAmount.h"
#include "ALSVLearnDemo/Interfaces/ALS_Animation.h"
#include "Animation/AnimInstance.h"
#include "Curves/CurveVector.h"
#include "ALSAnimInstance.generated.h"

class AALSVLearnDemoCharacter;

/*
 * 因为分别处理各个方向的值有点麻烦，所以就将值整合到了一个结构体中，函数的调用。
 */
USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_BODY()

public:
	FVelocityBlend(float f = 0.f, float b = 0.f, float l = 0.f, float r = 0.f) : F(f), B(b), L(l), R(r)
	{
	}

	// 变量分开写， 方便后面写注释
	// 一定要加上宏定义将变量公开，要不然无法使用Split Struct Pin.
	// 四个方向的运动速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float B;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float R;
};

/**
 * 
 */
UCLASS()
class ALSVLEARNDEMO_API UALSAnimInstance : public UAnimInstance, public IALS_Animation
{
	GENERATED_BODY()
public:
	UALSAnimInstance();

	/*
	 * 这两个函数创建动画实例的标配，一定要记住。
	 */
	virtual void NativeBeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 储存角色变量
	UPROPERTY(BlueprintReadOnly)
	AALSVLearnDemoCharacter* Character;

	/*
	* 更新角色相关变量
	*/
	UFUNCTION(BlueprintCallable)
	void UpdateCharacterInfo();

	/*
	 * 更新角色运动相关的值
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateMovementValues();

	/*
	 * 角色移动的速度
	 */
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;

	/*
	 * 计算各个方向的速度值
	 */
	UFUNCTION(BlueprintCallable)
	FVelocityBlend CalculateVelocityBlend();

	/*
	 * 对各个方向移动的差值计算。
	 */
	UFUNCTION(BlueprintCallable)
	FVelocityBlend InterpVelocityBlend(FVelocityBlend Current, FVelocityBlend Target, float InterpSpeed,
	                                   float DeltaTime) const;

	/*
	 * 计算步幅
	 */
	UFUNCTION(BlueprintCallable)
	float CalculateStrideBlend() const;

	///////////////////////////////////////////////////////////////////////////
	/*
	 * 判断是否处于移动状态
	 */
	UPROPERTY(BlueprintReadOnly, Category="Character Information")
	bool bIsMoving;

	/*
	 * 判断是否有移动按键输入
	 */
	UPROPERTY(BlueprintReadOnly, Category="Character Information")
	bool bHasMovementInput;

	/*
	 * 角色水平移动速度
	 */
	UPROPERTY(BlueprintReadOnly, Category="Character Information")
	float Speed;

	/**
	 * @brief 保存动画当前运动状态
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character Information")
	EALS_Gait Gait;

	/**
	 * @brief 储存人物是处于蹲伏、行走状态。
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character Information")
	EALS_Stance Stance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Information")
	EALS_RotationMode RotationMode;

	///////////////////////////////////////////////////////////////// 第八次更新

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character Information")
	EALS_MovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character Information")
	EALS_MovementAction MovementAction;

	///////////////////////////////////////////////////////////////// 

	/**
	 * @brief 角色控制的旋转值
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character Information")
	FRotator AimingRotation;

	/**
	 * @brief 当前控制器旋转速率
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character Information")
	float AimYawRate;

	/**
	 * @brief 加速度，但并不是真正的物理加速度。
	 */
	UPROPERTY(BlueprintReadOnly, Category="Character Information")
	FVector Acceleration;

	///////////////////////////////////////////////////////////////////////
	/// Anim Graph - Grounded


	/*
	* 判断是否可以移动
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|Grounded")
	bool bShouldMove;

	/*
	 * 储存各个个方向速度值
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|Grounded")
	FVelocityBlend VelocityBlend;

	/**
	 * @brief 相对加速度量
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|Grounded")
	FVector RelativeAccelerationAmount;

	/**
	 * @brief 身体倾斜度
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|Grounded")
	FLeanAmount LeanAmount;

	/**
	 * @brief 行走和跑步之间的过渡值
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Graph|Grounded")
	float WalkRunBlend;

	/**
	 * @brief 站立状态下的播放速率
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Graph|Grounded")
	float StandingPlayRate;

	/**
	 * @brief 蹲伏状态下的播放速率
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Graph|Grounded")
	float CrouchingPlayRate;

	/*
	 * 每次移动的步幅值
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|Grounded")
	float StrideBlend;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|Grounded")
	EGroundedEntryState GroundedEntryState;


	////////////////////////////////////////////////////////////////////////////////////
	/// Anim Graph - Aiming Values


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Aiming Values")
	FRotator SmoothedAimingRotation;

	/**
	 * @brief 玩家控制器和角色之间的差值
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Aiming Values")
	FVector2D AimingAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Aiming Values")
	FVector2D SmoothedAimingAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Aiming Values")
	float AimSweepTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Aiming Values")
	FRotator SpineRotation;


	/////////////////////////////////////////////////////////////////////////////////////
	/// Anim Graph - Grounded

	/*
	 * 旋转范围
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	float RotationScale;

	/**
	 * @brief 运动方向
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	EMovementDirection MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	float FYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	float BYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	float LYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	float RYaw;


	/////////////////////////////////////////////////////////////////////////////////////
	/// Anim Graph - InAir

	/*
	 * 判断是否处于跳跃状态
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|InAir")
	bool bJumped;

	/**
	 * @brief 跳跃动画播放速率
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|InAir")
	float JumpPlayRate;

	/**
	 * @brief 下落速度
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|InAir")
	float FallSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|InAir")
	float LandPrediction;

	/////////////////////////////////////////////////////////////////////////////////////
	/// InAir

	UFUNCTION(BlueprintCallable, Category="InAir")
	float CalculateLandPrediction();

	/**
	 * @brief 使用相对速度方向和数量确定角色在空气中应倾斜的程度。空气中倾斜曲线获得下降速度，并用作倍增器，以便在从向上移动过渡到向下移动时平滑反转倾斜方向。
	 */
	UFUNCTION(BlueprintCallable, Category="InAir")
	FLeanAmount CalculateInAirLeanAmount() const;

	/////////////////////////////////////////////////////////////////////////////////////
	/// Grounded


	/*
	 * 判断是否在移动
	 */
	UFUNCTION(BlueprintCallable, Category="Grounded")
	bool ShouldMoveCheck() const;

	/**
	 * @brief 判断是否可以在原地旋转
	 */
	UFUNCTION(BlueprintCallable, Category="Grounded")
	bool CanTurnInPlace() const;

	/**
	 * @brief 播放原地旋转动画检查
	 */
	UFUNCTION(BlueprintCallable, Category="Grounded")
	void TurnInPlaceCheck();

	/**
	 * @brief 原地旋转逻辑实现
	 * @param TargetRotation 旋转到的目标旋转值
	 * @param PlayRateScale  播放动画的旋转速率比例
	 * @param StartTime 动画开始时间
	 * @param bOverrideCurrent 是否覆盖当前动画
	 */
	UFUNCTION(BlueprintCallable, Category="Grounded")
	void TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool bOverrideCurrent);

	UFUNCTION(BlueprintCallable, Category="Grounded")
	void DynamicTransitionCheck();

	UFUNCTION(BlueprintCallable, Category="Grounded")
	void PlayDynamicTransition(float ReTriggerDelay, FDynamicMontageParams Parameters);

	UPROPERTY()
	FTimerHandle TH_AnimationDuration;

	//////////////////////////////////////////////////////////////////////////////////
	/// Turn In Place

	/*
	 * 已经经过的延迟时间
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	float ElapsedDelayTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	bool bRotateL;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	bool bRotateR;

	/**
	 * @brief 最小旋转角度
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	float TurnCheckMinAngle;

	/**
	 * @brief 最大旋转速率
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	float AimYawRateLimit;

	/**
	 * @brief 旋转最小延迟时间
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	float MinAngleDelay;

	/**
	 * @brief 旋转最大延迟时间
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	float MaxAngleDelay;

	/**
	 * @brief 旋转阈值，超过了这个阈值就启用180旋转动画
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	float Turn180Threshold;

	///
	/// 旋转动画资源
	///

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn In Place")
	FTurnInPlace_Asset N_TurnIP_L90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn In Place")
	FTurnInPlace_Asset N_TurnIP_R90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn In Place")
	FTurnInPlace_Asset N_TurnIP_L180;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn In Place")
	FTurnInPlace_Asset N_TurnIP_R180;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn In Place")
	FTurnInPlace_Asset CLF_TurnIP_L90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn In Place")
	FTurnInPlace_Asset CLF_TurnIP_R90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn In Place")
	FTurnInPlace_Asset CLF_TurnIP_L180;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn In Place")
	FTurnInPlace_Asset CLF_TurnIP_R180;

	////////////////////////////////////////////////////////////////////////////////
	/// Update Values


	/*
	 * 更新瞄准相关的值
	 */
	UFUNCTION(BlueprintCallable, Category="Update Values")
	void UpdateAimingValues();

	UFUNCTION(BlueprintCallable, Category="Update Values")
	void UpdateRotationValues();

	UFUNCTION(BlueprintCallable, Category="Update Values")
	void UpdateInAirValues();

	////////////////////////////////////////////////////////////////////////////////
	/// Rotation 函数

	/*
	 * 计算运动方向。该值表示角色在“环视/瞄准”旋转模式期间相对于摄影机移动的方向，并在循环混合动画层中用于混合到适当的方向状态。
	 */
	UFUNCTION(BlueprintCallable, Category="Rotation")
	EMovementDirection CalculateMovementDirection();

	/**
	 * @brief 获取输入角度并确定其象限（方向）。使用当前移动方向增加或减少每个象限角度范围上的缓冲区。
	 */
	UFUNCTION(BlueprintCallable, Category="Rotation")
	EMovementDirection CalculateQuadrant(EMovementDirection Current, float FR_Threshold, float FL_Threshold,
	                                     float BR_Threshold, float BL_Threshold, float Buffer, float Angle);

	UFUNCTION(BlueprintCallable, Category="Rotation")
	bool AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool bIncreaseBuffer);

	////////////////////////////////////////////////////////////////////////////////
	/// 配置相关变量

	/*
	 * 各个方向进行差值计算更新的速度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float VelocityBlendInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float InAirLeanInterpSpeed;
	
	/**
	 * @brief 动画行走速度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float AnimatedWalkSpeed;

	/**
	 * @brief 动画跑步速度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float AnimatedRunSpeed;

	/**
	 * @brief 动画蹲伏速度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float AnimatedCrouchSpeed;

	/**
	 * @brief 动画跑步速度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float AnimatedSprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float SmoothedAimingRotationInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float GroundedLeanInterpSpeed;


	/////////////////////////////////////////////////////////////////////////////////
	///混合曲线相关变量

	/*
	 * 向前运动的走路运动曲线
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend Curves")
	UCurveFloat* StrideBlend_N_Walk;

	/*
	 * 向前运动的跑步运动曲线
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend Curves")
	UCurveFloat* StrideBlend_N_Run;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend Curves")
	UCurveFloat* LandPredictionCurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend Curves")
	UCurveFloat* LeanInAirCurveFloat;

	/*
	 * 蹲伏运动曲线
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend Curves")
	UCurveFloat* StrideBlend_C_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend Curves")
	UCurveVector* YawOffset_FB;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend Curves")
	UCurveVector* YawOffset_LR;

	/////////////////////////////////////////////////////////////////////////////////
	/// 运动相关函数

	/*
	 * 计算相对加速度量
	 */
	UFUNCTION(BlueprintCallable, Category="Movement")
	FVector CalculateRelativeAccelerationAmount();

	/*
	 * 计算行走和跑步之间数值
	 */
	UFUNCTION(BlueprintCallable, Category="Movement")
	float CalculateWalkRunBlend();

	/**
	 * @brief 通过将角色的速度除以每个步态的动画速度来计算播放速率。
	 * 插值由存在于每个运动循环中的“Weight_Gait”动画曲线确定，因此播放速率始终与当前混合动画同步。
	 * 该值还除以步幅混合和网格体比例，以便随着步幅或比例变小，播放速率增加。
	 */
	UFUNCTION(BlueprintCallable, Category="Movement")
	float CalculateStandingPlayRate();

	UFUNCTION(BlueprintCallable, Category="Movement")
	float CalculateCrouchingPlayRate();


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Interpolation

	static FLeanAmount InterpLeanAmount(FLeanAmount Current, FLeanAmount Target, float InterpSpeed, float DeltaTime);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// References

	/*
	* 世界增量时间
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="References")
	float DeltaTimeX;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Additive Transitions

	/*
	 * 播放过渡动画
	 */
	UFUNCTION(BlueprintCallable)
	void PlayTransition(FDynamicMontageParams Parameters);


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 接口函数

	virtual void API_SetGroundedEntryState_Implementation(EGroundedEntryState m_GroundedEntryState) override;

	virtual void API_Jumped_Implementation() override;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Delay 延迟相关函数、变量

	/*
	 * 跳跃延迟的时间记录
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Delay")
	FTimerHandle TH_Jumped;

	/**
	 * @brief 重置跳跃状态
	 */
	UFUNCTION(BlueprintCallable, Category="Delay")
	void ResetJumped();

protected:
	/*
	 * 获得动画曲线的值
	 */
	UFUNCTION()
	float GetAnimCurveClamped(FName Name, float Bias, float ClampMin, float ClampMax) const;
};
