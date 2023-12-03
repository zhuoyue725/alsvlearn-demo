// Copyright © 2021 Tanzq. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ALSVLearnDemo/Data/Enums/MovementDirection.h"
#include "ALSVLearnDemo/Data/Structs/ALS_MovementState.h"
#include "ALSVLearnDemo/Data/Structs/LeanAmount.h"
#include "ALSVLearnDemo/Data/Enums/ALS_RotationMode.h"
#include "ALSVLearnDemo/Data/Enums/ALS_MovementAction.h"
#include "ALSVLearnDemo/Data/Enums/ALS_Stance.h"
// #include "ALSVLearnDemo/Data/Structs/FVelocityBlend.h"
// #include "ALSVLearnDemo/Character/ALSAnimInstance.h"
#include "../Data/Enums/ALS_Gait.h"
#include "Curves/CurveVector.h"
#include "ALSVLearnDemo/Data/Structs/TurnInPlace_Asset.h"
#include "ALSVLearnDemo/Interfaces/ALS_Animation.h"
#include "ACSAnimInstance.generated.h"

class AALSVLearnDemoCharacter;

USTRUCT(BlueprintType)
struct FVelocityBlend2
{
	GENERATED_BODY()

public:
	FVelocityBlend2(float f = 0.f, float b = 0.f, float l = 0.f, float r = 0.f) : F(f), B(b), L(l), R(r)
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
class ALSVLEARNDEMO_API UACSAnimInstance : public UAnimInstance,public IALS_Animation
{
	GENERATED_BODY()

public:
	UACSAnimInstance();

	/*
	* 这两个函数创建动画实例的标配，一定要记住。
	*/
	UFUNCTION(BlueprintCallable)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeBeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateCharacterInfo();

	UFUNCTION(BlueprintCallable)
	void UpdateAimingValues();

	UFUNCTION(BlueprintCallable)
	float CalculateStrideBlend();

	UFUNCTION(BlueprintCallable)
	float CalculateWalkRunBlend();

	UFUNCTION(BlueprintCallable)
	void UpdateMovementValues();

	UFUNCTION(BlueprintCallable)
	void UpdateRotationValues();

	UFUNCTION(BlueprintCallable)
	float CalculateStandingPlayRate();

	UFUNCTION(BlueprintCallable)
	EMovementDirection CalculateMovementDirection();

	UFUNCTION(BlueprintCallable)
	EMovementDirection CalculateQuadrant(EMovementDirection Current, float FR_Threshold,float FL_Threshold, float BR_Threshold, float BL_Threshold,float Buffer, float Angle);

	UFUNCTION(BlueprintCallable)
	FVelocityBlend2 CalculateVelocityBlend();

	UFUNCTION(BlueprintCallable)
	FVector CalculateRelativeAccelerationAmount();
	
	UFUNCTION(BlueprintCallable)
	bool AngleInRange(float Angle, float FL_Threshold, float FR_Threshold, float Buffer, bool bShouldBuffer);
	
	UFUNCTION(BlueprintCallable)
	bool ShouldMoveCheck();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="Foot IK")
	void UpdateFootIK();
	virtual void UpdateFootIK_Implementation();
	/*
	* 对各个方向移动的差值计算。
	*/
	UFUNCTION(BlueprintCallable)
	FVelocityBlend2 InterpVelocityBlend(FVelocityBlend2 Current, FVelocityBlend2 Target, float InterpSpeed,
									   float DeltaTime) const;

	UFUNCTION(BlueprintCallable)
	float GetAnimCurveClamped(FName Name, float Bias, float ClampMin, float ClampMax) const;

	UFUNCTION(BlueprintCallable)
	void TurnInPlaceCheck();

	UFUNCTION(BlueprintCallable)
	void TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool bOverrideCurrent);

	UFUNCTION(BlueprintCallable)
	FLeanAmount InterpLeanAmount(FLeanAmount Current,FLeanAmount Target,float DeltaTime,float InterpSpeed);
	
	UFUNCTION(BlueprintCallable)
	bool CanTurnInPlace();

	UFUNCTION(BlueprintCallable, Category="InAir")
	void ResetJumped();

	UFUNCTION(BlueprintCallable, Category="InAir")
	float CalculateLandPrediction();

	UFUNCTION(BlueprintCallable, Category="InAir")
	FLeanAmount CalculateInAirLeanAmount() const;
	
	UPROPERTY(BlueprintReadOnly)
	AALSVLearnDemoCharacter* Character;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 接口函数

	virtual void API_SetGroundedEntryState_Implementation(EGroundedEntryState m_GroundedEntryState) override;

	virtual void API_Jumped_Implementation() override;
	
	/////////////////////////////////////////////////////////////////////////// 角色运动数据

	// 旋转相关
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationYaw;
	
	// 从站立到目标状态的速度变化（加速过渡）曲线
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
	UCurveFloat* StrideBlend_N_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
	UCurveFloat* StrideBlend_C_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
	UCurveFloat* StrideBlend_N_Run;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend Curves")
	UCurveVector* YawOffset_FB;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Blend Curves")
	UCurveVector* YawOffset_LR;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend Curves")
	UCurveFloat* LeanInAirCurveFloat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	float FYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	float BYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	float LYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	float RYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float AnimatedWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float AnimatedRunSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float AnimatedSprintSpeed;

	/*
	 * 转向资源
	 */
	// 蹲下
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	FTurnInPlace_Asset CLF_TurnIP_L180;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	FTurnInPlace_Asset CLF_TurnIP_R180;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	FTurnInPlace_Asset CLF_TurnIP_L90;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	FTurnInPlace_Asset CLF_TurnIP_R90;

	// 站立
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	FTurnInPlace_Asset N_TurnIP_L180;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	FTurnInPlace_Asset N_TurnIP_R180;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	FTurnInPlace_Asset N_TurnIP_L90;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	FTurnInPlace_Asset N_TurnIP_R90;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	FVector2D AimingAngle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	float TurnCheckMinAngle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Turn In Place")
	float AngleDelayTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	float ElapsedDelayTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	float RotationScale;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Turn In Place")
	float Turn180Threshold;
	/*
	 * 储存各个个方向速度值
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|Grounded")
	FVelocityBlend2 VelocityBlend;

	/*
	* 各个方向进行差值计算更新的速度
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float VelocityBlendInterpSpeed;

	/*
	* 世界增量时间
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="References")
	float DeltaTimeX;
	
	UPROPERTY(BlueprintReadOnly)
	bool bShouldMove;
	/*
	 * 角色移动的速度
	 */
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;
	/*
	 * 判断是否处于移动状态
	 */
	UPROPERTY(BlueprintReadOnly, Category="Character Information")
	bool bIsMoving;
	/**
	 * @brief 加速度，但并不是真正的物理加速度。
	 */
	UPROPERTY(BlueprintReadOnly, Category="Character Information")
	FVector Acceleration;
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

	/*
	* 步幅
	*/
	UPROPERTY(BlueprintReadOnly, Category="Character Information")
	float StrideBlend;
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

	////////////////////////////////////////////////////////////////////////////运动状态
	// 移动状态：Grounded
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character Information")
	EALS_MovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character Information")
	EALS_MovementAction MovementAction;
	/**
	* @brief 储存人物是处于蹲伏、行走状态。
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character Information")
	EALS_Stance Stance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State Values")
	// 步态：跑步or走路or冲刺
	EALS_Gait Gait;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Graph|Grounded")
	// 步态：跑步or走路or冲刺
	EALS_RotationMode RotationMode;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Graph|Grounded")
	FVector RelativeAccelerationAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Graph|Grounded")
	FLeanAmount LeanAmount;
	
	/**
	 * @brief 运动方向
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|Grounded")
	EMovementDirection MovementDirection;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|Grounded")
	EGroundedEntryState GroundedEntryState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Anim Graph|InAir")
	bool bJumped;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|InAir")
	float JumpPlayRate;
	
	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|InAir")
	float FallSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph|InAir")
	float LandPrediction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	float GroundedLeanInterpSpeed;

	/*
	 * 跳跃延迟的时间记录
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Delay")
	FTimerHandle TH_Jumped;
};

