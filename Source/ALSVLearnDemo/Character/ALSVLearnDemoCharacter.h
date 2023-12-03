// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ALSAnimInstance.h"
#include "../Data/Structs/MovementSettings.h"
#include "ALSVLearnDemo/Data/Enums/ALS_Stance.h"
#include "ALSVLearnDemo/Data/Structs/ALS_MovementState.h"
#include "ALSVLearnDemo/Interfaces/ALS_Camera.h"
#include "ALSVLearnDemo/Interfaces/ALS_Character.h"
#include "GameFramework/Character.h"
#include "ALSVLearnDemoCharacter.generated.h"

UCLASS(config=Game)
class AALSVLearnDemoCharacter : public ACharacter, public IALS_Camera, public IALS_Animation, public IALS_Character
{
	GENERATED_BODY()

public:
	AALSVLearnDemoCharacter();

	virtual void BeginPlay() override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;
	// End of APawn interface

public:
	////////////////////////////////////////////////////////////////////////////////
	/// 输入相关变量

	/** Called for forwards/backward input */
	UFUNCTION()
	void MoveForward(float Value);

	/** Called for side to side input */
	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void LookUp(float Value);

	UFUNCTION()
	void LookRight(float Value);

	/**
	 * @brief 更换移动和跑步动作
	 */
	UFUNCTION()
	void MoveWalk();

	/**
	 * @brief 按下冲刺按键之后的逻辑实现
	 */
	UFUNCTION()
	void MoveSprint_Pressed();

	/**
	 * @brief 松开冲刺按键之后的逻辑实现
	 */
	UFUNCTION()
	void MoveSprint_Released();

	/*
	 * 按一次切换行走和蹲伏，连续按两次 —> 滚动前进。
	 */
	UFUNCTION(BlueprintCallable)
	void ResetStanceActionCount();
	
	/*
	 * 记录按下StanceAction多少次的函数。
	 */
	UFUNCTION(BlueprintCallable)
	void StanceAction();

	/**
	* @brief 向上向下的速率
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Input")
	float LookUpRate;

	/**
	 * @brief 向左向右的速率
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Input")
	float LookRightRate;

	/**
	 * @brief 要变成的步态，也就是对我们按下的键进行保存
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	EALS_Gait DesiredGait;

	/**
	 * @brief 将要变成的Stance状态
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	EALS_Stance DesiredStance;

	//////////////////////////////////////////////////////////////////////////////
	/// Crouch
	
	// 重载蹲伏函数
	// TODO 到底谁会调用它呢？？ 加了和没加有什么区别？
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	/////////////////////////////////////////////////////////////////////  第八次更新
	/*
	* 储存按键时间
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Crouch")
	FTimerHandle TH_StanceAction;

	/**
	 * @brief 在规定时间内按下StanceAction键的次数
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Crouch")
	int32 StanceActionCount;

	///////////////////////////////////////////////////////////////////// 

	/////////////////////////////////////////////////////////////////////////////
	/// Utility

	/*
	 * 获得动画实例曲线对应的值
	 */
	UFUNCTION(BlueprintCallable, Category="Utility")
	float GetAnimCurveValue(FName CurveName);

	//////////////////////////////////////////////////////////////////////////////
	///人物相关状态变量 

	// 判断角色是否处于运动状态
	UPROPERTY(BlueprintReadOnly, Category="Essential Information")
	bool bIsMoving;

	// 判断是否有运动输入
	UPROPERTY(BlueprintReadOnly, Category="Essential Information")
	bool bHasMovementInput;

	// 角色移动的水平速度，就是不包含Z轴的速度大小。
	UPROPERTY(BlueprintReadOnly, Category="Essential Information")
	float Speed;

	/**
	 * @brief 瞄准镜头移动速率
	 */
	UPROPERTY(BlueprintReadOnly, Category="Essential Information")
	float AimYawRate;

	// 按键输入比率
	UPROPERTY(BlueprintReadOnly, Category="Essential Information")
	float MovementInputAmount;

	/**
	 * @brief 目标旋转值
	 */
	UPROPERTY(BlueprintReadOnly, Category="Essential Information")
	FRotator TargetRotation;

	/**
	 * @brief 上一个延迟差运动速度的旋转值
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Essential Information")
	FRotator LastVelocityRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Essential Information")
	FRotator LastMovementInputRotation;

	/**
	 * @brief 加速度，但并不是真正的物理加速度。
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Essential Information")
	FVector Acceleration;
	
	/**
	 * @brief 获得角色的运动状态值
	 */
	virtual void API_GetCurrentStates_Implementation(EALS_MovementState& m_MovementState, EALS_MovementAction& m_MovementAction, EALS_Gait& m_Gait, EALS_Stance& m_Stance, EALS_RotationMode& Eals_RotationMode) override;

	/**
	 * @brief 保存缓存数据
	 */
	UFUNCTION(BlueprintCallable, Category="Essential Information")
	void CacheValues();

	UFUNCTION(BlueprintCallable, Category="Jump Action")
	void JumpAction();

	// BlueprintImplementableEvent是在蓝图中重载，而不是C++
	// BlueprintNativeEvent可以在蓝图重写，也可以在C++实现，但是需要定义一个virtual的带有后缀_Implementation的函数
	// BlueprintCallable可以让子类调用父类的方法
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Mantle Function")
	bool MantleCheck();
	virtual bool MantleCheck_Implementation();
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Mantle Function")
	void MantleUpdate();
	virtual void MantleUpdate_Implementation();
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category="Mantle Function")
	void MantleStart();
	virtual void MantleStart_Implementation();
	
	// 获取人物状态相关的值
	virtual void API_GetEssentialValues_Implementation(FVector& Velocity, FVector& m_Acceleration, bool& IsMoving, bool& HasMovementInput, float& fSpeed, FRotator& AimingRotation, float& m_AimYawRate) override;

	// 更新角色运动相关变量
	UFUNCTION(BlueprintCallable, Category="Essential Information")
	void SetEssentialValues();
	
	/**
	 * @brief 通过比较当前和以前的速度来计算加速度。运动组件返回的当前加速度等于输入加速度，但并不表示角色的实际物理加速度。
	 */
	UFUNCTION(BlueprintCallable, Category="Essential Information")
	FORCEINLINE FVector CalculateAcceleration() const { return (GetVelocity() - PreviousVelocity) / GetWorld()->DeltaTimeSeconds;};


	//////////////////////////////////////////////////////////////////////////////////////
	/// Cached Variables

	/*
	 * 上一个延迟差运动的速度
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Cached Variables")
	FVector PreviousVelocity;

	/**
	 * @brief 上一个延迟时间控制器Yaw值的变化量
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Cached Variables")
	float PreviousAimYaw;

	//////////////////////////////////////////////////////////////////////////////////
	/// 状态相关变量


	/**
	* @brief 步态
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State Values")
	EALS_Gait Gait;

	/**
	 * @brief 储存角色 蹲伏、行走状态。
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State Values")
	EALS_Stance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State Values")
	EALS_MovementState MovementState;

	/**
	 * @brief 人物动作行为状态
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State Values")
	EALS_MovementAction MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State Values")
	EALS_RotationMode RotationMode;
	
	/**
	 * @brief 更新步态
	 */
	UFUNCTION(BlueprintCallable, Category="State Values")
	void SetGait(EALS_Gait NewGait);

	////////////////////////////////////////////////////////////////////////////////////
	///  状态改变


	UFUNCTION(BlueprintCallable, Category="State Changes")
	void OnBeginPlay();

	////////////////////////////////////////////////////////// 第八次更新

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	UFUNCTION(BlueprintCallable, Category="State Changes")
	void OnCharacterMovementModeChanged(EMovementMode PrevMovementMode, EMovementMode NewMovementMode, uint8 PrevCustomMode, uint8 NewCustomMode);

	UFUNCTION(BlueprintCallable, Category="State Changes")
	void SetMovementState(EALS_MovementState NewMovementState);

	////////////////////////////////////////////////////////// 

	/////////////////////////////////////////////////////////////////////////////////
	/// 引用相关变量

	/*
	 * 动画实例
	 */
	UPROPERTY(BlueprintReadOnly)
	UAnimInstance* MainAnimInstance;

	/////////////////////////////////////////////////////////////////////////////////
	/// 人物运动

	/**
	* @brief 储存的是当前运动数值的设置
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement System")
	FMovementSettings CurrentMovementSettings;

	/**
	 * @brief 运动数据
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement System")
	FMovementSettingsState MovementData;

	/*
	 * 更新人物运动相关变量
	 */
	UFUNCTION(BlueprintCallable, Category="Movement System")
	void UpdateCharacterMovement();

	//////////////////////////////////////////////////////////////////// 第八次更新

	/*
	 * 这个动画内容由子类来定义。
	 */
	UFUNCTION(BlueprintCallable, Category="Movement System")
	virtual UAnimMontage* GetRollAnimation();

	//////////////////////////////////////////////////////////////////// 

	/**
	 * @brief 获得当前状态下的运动数据
	 */
	UFUNCTION(BlueprintCallable, Category="Movement System")
	FMovementSettings GetTargetMovementSettings();

	/**
	 * @brief  获得实际的步态。这是由角色的实际运动计算的，因此它可能不同于所需步态或允许步态。例如，如果允许的步态变为行走，则实际步态仍将运行，直到角色减速到行走速度。
	 */
	UFUNCTION(BlueprintCallable, Category="Movement System")
	EALS_Gait GetActualGait(EALS_Gait AllowedGait);

	/**
	 * @brief 根据数据名称在数据表中提取数据
	 * @param RowName 数据名称
	 */
	UFUNCTION(BlueprintCallable, Category="Movement System")
	void SetMovementModel(FName RowName);

	/**
	 * @brief 动态更新目前状态对应的速度
	 */
	UFUNCTION(BlueprintCallable, Category="Movement System")
	void UpdateDynamicMovementSettings(EALS_Gait AllowedGait);

	/**
	 * @brief 用速度值取对应时间轴的区域
	 * @return 曲线时间
	 */
	UFUNCTION(BlueprintCallable, Category="Movement System")
	float GetMappedSpeed();

	/**
	 * @brief 获取可以进行的步态
	 */
	UFUNCTION(BlueprintCallable, Category="Movement System")
	EALS_Gait GetAllowedGait();

	/**
	 * @brief 判断是否可以冲刺
	 */
	UFUNCTION(BlueprintCallable, Category="Movement System")
	bool CanSprint();

	/////////////////////////////////////////////////////////////////////////////////
	/// 人物旋转相关函数

	/*
	 * 角色在空中的旋转量
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Rotation System")
	FRotator InAirRotation;
	
	UFUNCTION(BlueprintCallable, Category="Rotation System")
	void UpdateGroundedRotation();

	
	/**
	 * @brief 判断是否在运动
	 */
	UFUNCTION(BlueprintCallable, Category="Rotation System")
	bool CanUpdateMovingRotation();

	/**
	 * @brief 插值目标旋转以获得额外的平滑旋转行为
	 */
	UFUNCTION(BlueprintCallable, Category="Rotation System")
	void SmoothCharacterRotation(FRotator Target, const float TargetInterpSpeed, const float ActorInterpSpeed);

	UFUNCTION(BlueprintCallable, Category="Rotation System")
	void UpdateInAirRotation();

	/**
	 * @brief 使用“移动设置”中的当前旋转速率曲线计算旋转速率。将曲线与映射速度结合使用，可以对每个速度的转速进行高度控制。如果相机快速旋转，请提高速度，以获得更灵敏的旋转。
	 */
	UFUNCTION(BlueprintCallable, Category="Rotation System")
	float CalculateGroundedRotationRate();

	/////////////////////////////////////////////////////////////////////////////////
	/// 摄像机相关变量、函数

	UPROPERTY(Category="Camera System", EditInstanceOnly, BlueprintReadWrite)
	float ThirdPersonFOV;

	UPROPERTY(Category="Camera System", EditInstanceOnly, BlueprintReadWrite)
	float FirstPersonFOV;

	UPROPERTY(Category="Camera System", EditInstanceOnly, BlueprintReadWrite)
	bool bRightShoulder;

	/////////////////////////////////////////////////////////////////////////////////
	/// 接口相关变量、函数

	virtual bool GetCameraParameters_Implementation(float& TP_FOV, float& FP_FOV) override;

	virtual void Get3PTraceParams_Implementation(FVector& TraceOrigin, float& TraceRadius, TEnumAsByte<ETraceTypeQuery>& TraceChanel) override;	// virtual void Get3PTraceParams_Implementation(FVector& TraceOrigin, float& TraceRadius, TEnumAsByte<ETraceTypeQuery>& TraceChanel) override;

	// virtual void OnJumped_Implementation() override;
	virtual void OnJumped_Implementation() override;
	
	////////////////////////////////////////////////////////////////////////////////
	/// State Changes

	virtual void API_SetMovementAction_Implementation(EALS_MovementAction m_MovementAction) override;

protected:
	/**
	 * @brief 运动相关数据表格
	 */
	UPROPERTY(EditDefaultsOnly, Category="Movement System")
	class UDataTable* MovementModelDB;
};
