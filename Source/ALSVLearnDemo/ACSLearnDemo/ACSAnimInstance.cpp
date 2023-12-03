// Copyright © 2021 Tanzq. All rights reserved.
#include "ACSAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "../Character/ALSVLearnDemoCharacter.h"
#include "ALSVLearnDemo/Data/Enums/ALS_RotationMode.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Lumin/CAPIShims/LuminAPIImpl.h"

UACSAnimInstance::UACSAnimInstance()
{
	VelocityBlendInterpSpeed = 12.f;
	AnimatedWalkSpeed = 150.f;
	AnimatedRunSpeed = 350.f;
	AnimatedSprintSpeed = 600.f;

	// 转向
	TurnCheckMinAngle = 30.0f;
	Turn180Threshold = 130.0f;

	// 默认旋转模式
	RotationMode = EALS_RotationMode::EARm_LookingDirection;

	// 倾斜插值速度
	GroundedLeanInterpSpeed = 4.f;
}

void UACSAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	MovementState = EALS_MovementState::EAMs_Grounded;
	Character = Cast<AALSVLearnDemoCharacter>(TryGetPawnOwner());
	if(!Character)
	{
		UE_LOG(LogTemp,Warning,TEXT("Character Not Set"));
	}
	
}

void UACSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	// 更新世界增量值
	DeltaTimeX = DeltaSeconds;
	
	// UE_LOG(LogTemp,Warning,TEXT("NativeUpdateAnimation"));
	if (DeltaTimeX == 0.f || !Character) return;

	// Do Every Frame
	// 从Character的API中获取角色当前信息
	UpdateCharacterInfo();

	// 更新镜头瞄准
	UpdateAimingValues();

	// 更新脚部IK
	UpdateFootIK();
	
	switch (MovementState)
	{
		bool bTemp;
	case EALS_MovementState::EAMs_None:
		break;
	case EALS_MovementState::EAMs_Grounded:
		bTemp = ShouldMoveCheck();
		bShouldMove = bTemp;
		// 角色在运动状态
		if (bTemp)
		{
			UpdateMovementValues();
			UpdateRotationValues();
		}
		else
		{
			// 原地旋转
			if (CanTurnInPlace())
			{
				// 如果可以原地旋转，执行原地旋转相关逻辑
				TurnInPlaceCheck();
			}
			else
			{
				ElapsedDelayTime = 0.f;
			}
		}
		break;
	case EALS_MovementState::EAMs_InAir:
		break;
	case EALS_MovementState::EAMs_Mantling:
		break;
	case EALS_MovementState::EAMs_Ragdoll:
		break;
	default:
		break;
	}
}

void UACSAnimInstance::UpdateMovementValues()
{
	// 获取各个方向下帧的混合速度
	VelocityBlend = InterpVelocityBlend(VelocityBlend, CalculateVelocityBlend(), VelocityBlendInterpSpeed, DeltaTimeX);

	// 更新stride、play rate、WalkRunBlend
	
	// 计算是行走还是跑步，按下ctrl是走路
	WalkRunBlend = CalculateWalkRunBlend();

	// 计算步幅
	StrideBlend = CalculateStrideBlend();

	// 更新站立状态下的播放速率
	StandingPlayRate = CalculateStandingPlayRate();

	// 冲刺需要的相对加速度（用于混合加速过程or加速后），前后加速度值归一化
	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();

	// 身体倾斜度
	LeanAmount = InterpLeanAmount(LeanAmount,FLeanAmount(RelativeAccelerationAmount.Y,RelativeAccelerationAmount.X),DeltaTimeX,GroundedLeanInterpSpeed);
}

void UACSAnimInstance::UpdateRotationValues()
{
	// 更新MovementDirection参数
	MovementDirection = CalculateMovementDirection();

	// Actor的旋转，如速度左前，将Actor朝向修改为正向左前
	const float Time = UKismetMathLibrary::NormalizedDeltaRotator(Velocity.ToOrientationRotator(),
																  Character->GetControlRotation()).Yaw;
	FVector CurveValue = YawOffset_FB->GetVectorValue(Time);
	FYaw = CurveValue.X;
	BYaw = CurveValue.Y;

	CurveValue = YawOffset_LR->GetVectorValue(Time);
	LYaw = CurveValue.X;
	RYaw = CurveValue.Y;
}

bool UACSAnimInstance::ShouldMoveCheck()
{
	return bIsMoving && bHasMovementInput || Speed > 150.0f;
	// return bHasMovementInput;
}

void UACSAnimInstance::UpdateFootIK_Implementation()
{
	UE_LOG(LogTemp,Log,TEXT("UpdateFootIK"));	
}

void UACSAnimInstance::UpdateCharacterInfo()
{
	const IALS_Character* Character_API = Cast<IALS_Character>(Character);

	// 更新角色运动数值
	Character_API->Execute_API_GetEssentialValues(Character, Velocity, Acceleration, bIsMoving, bHasMovementInput,
												  Speed, AimingRotation, AimYawRate);

	// 更新人物运动状态
	Character_API->Execute_API_GetCurrentStates(Character, MovementState, MovementAction, Gait, Stance , RotationMode);
}

void UACSAnimInstance::UpdateAimingValues()
{
	// 当前Actor朝向和镜头朝向的插值
	FRotator AimingDeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(AimingRotation,Character->GetActorRotation());
	AimingAngle = FVector2D(AimingDeltaRotation.Yaw,AimingDeltaRotation.Pitch);
}

////////////////////////////////////////////////////////////////////////////////////////////////// 计算逻辑

FVector UACSAnimInstance::CalculateRelativeAccelerationAmount()
{
	float MaxValue;
	// 速度与加速度方向点乘
	// > 0 同向 加速
	// == 0 垂直
	// < 0 异向 减速
	if(FVector::DotProduct(Velocity,Acceleration) > 0.f)
	{
		MaxValue = Character->GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{
		MaxValue = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();
	}

	// 转换到局部坐标系？？
	return Character->GetActorRotation().UnrotateVector(Acceleration.GetClampedToMaxSize(MaxValue) / MaxValue);
}

FVelocityBlend2 UACSAnimInstance::CalculateVelocityBlend()
{
	// 获取速度方向相对于角色来说反方向旋转【GetActorRotation()】的向量值
	const FVector LocRelativeVelocityDir = Character->GetActorRotation().UnrotateVector(Velocity.GetSafeNormal(.1f));
	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) + FMath::Abs(
		LocRelativeVelocityDir.Z);

	// UE_LOG(LogTemp,Display,TEXT("%f,%f,%f,V = %f"),Velocity.X,Velocity.Y,Velocity.Z,Velocity.Size());
	
	// UE_LOG(LogTemp,Warning,TEXT("%f,%f,%f,sum = %f"),Character->GetActorRotation().Pitch,Character->GetActorRotation().Yaw,
	// Character->GetActorRotation().Roll,Sum);
	
	// 计算出各个方向所占的百分比 
	const FVector RelativeDirection = LocRelativeVelocityDir / Sum;

	return FVelocityBlend2(
		FMath::Clamp(RelativeDirection.X, 0.f, 1.f), // 向前运动速度百分比
		FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.f, 0.f)), // 向后运动速度百分比
		FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.f, 0.f)), // 向左运动速度百分比
		FMath::Clamp(RelativeDirection.Y, 0.f, 1.f)); // 向右运动速度百分比
}

// 各个方向速度的差值计算
FVelocityBlend2 UACSAnimInstance::InterpVelocityBlend(FVelocityBlend2 Current, FVelocityBlend2 Target, float InterpSpeed,
													 float DeltaTime) const
{
	return FVelocityBlend2(
		FMath::FInterpTo(Current.F, Target.F, DeltaTime, InterpSpeed),
		FMath::FInterpTo(Current.B, Target.B, DeltaTime, InterpSpeed),
		FMath::FInterpTo(Current.L, Target.L, DeltaTime, InterpSpeed),
		FMath::FInterpTo(Current.R, Target.R, DeltaTime, InterpSpeed));
}

/**
* @brief 根据Weight_Gait、BasePose_CLF曲线值取判断该取StrideBlend_N_Walk、StrideBlend_N_Run、StrideBlend_C_Walk那个曲线的值
* Weight_Gait == 1， 取StrideBlend_N_Walk的值
* Weight_Gait == 2、3， 取StrideBlend_N_Run的值，
* 如果有蹲伏曲线BasePose_CLF == 1，那么就取StrideBlend_C_Walk的值。
* 这里说的是端点取值，但是实际情况并不是都在端点，所以就使用差值函数进行平滑的过渡。
*/
float UACSAnimInstance::CalculateStrideBlend()
{
	// 每个动画（走路、跑、冲刺）对Weight_Gait进行了设置，分别对应（1、2、3），手动设置感觉也行但是可能更复杂
	float strideBlend_Walk = StrideBlend_N_Walk->GetFloatValue(Speed);
	float strideBlend_Run = StrideBlend_N_Run->GetFloatValue(Speed);
	float alpha = GetAnimCurveClamped(TEXT("Weight_Gait"),-2.f,0.f,1.f);
	float gaitValue = UKismetMathLibrary::Lerp(strideBlend_Walk,strideBlend_Run,alpha);

	float clfValue = StrideBlend_C_Walk->GetFloatValue(Speed);
	// 考虑下蹲的情况
	return UKismetMathLibrary::Lerp(gaitValue,clfValue,
		GetCurveValue(TEXT("BasePose_CLF")));
}

// 步态
float UACSAnimInstance::CalculateWalkRunBlend()
{
	if (Gait == EALS_Gait::EAG_Walking)
	{
		return 0.0f;
	}
	return 1.0f;
}
bool UACSAnimInstance::CanTurnInPlace()
{
	// 人物曲线 “Enable_Transition” 判断是否可以进行动作过渡
	// 旋转的时候的值是 < 0.99的，因此只会转向一次直到停止，但是不知道咋改的
	// TODO 还有两个条件没有加上去
	return GetCurveValue(TEXT("Enable_Transition")) > 0.99f;
}
// 播放速率，使得动画速率和胶囊体移动相匹配，避免滑步现象(主要是加速的过程)
float UACSAnimInstance::CalculateStandingPlayRate()
{
	// walk 还是 run？
	float walkRate = Speed / AnimatedWalkSpeed;
	float runRate = Speed / AnimatedRunSpeed;
	float gaitRate = UKismetMathLibrary::Lerp(walkRate, runRate,GetAnimCurveClamped(TEXT("Weight_Gait"), -1.f, 0.f, 1.f));
	// sprint？
	float sprintSpeed = Speed / AnimatedSprintSpeed;
	float sprintRate = UKismetMathLibrary::Lerp(gaitRate, sprintSpeed,GetAnimCurveClamped(TEXT("Weight_Gait"), -2.f, 0.f, 1.f));
	return FMath::Clamp(sprintRate / StrideBlend / GetOwningComponent()->GetComponentScale().Z,0.f,3.f);
}

// 旋转
EMovementDirection UACSAnimInstance::CalculateMovementDirection()
{
	if (RotationMode == EALS_RotationMode::EARm_VelocityDirection)
	{
		return EMovementDirection::EMD_Forward;
	}
	// 计算控制器方向和速度方向的夹角，区分4种情况
	return CalculateQuadrant(MovementDirection,70.f,-70.f,110.f,-110.f,5.f,
		UKismetMathLibrary::NormalizedDeltaRotator(Velocity.ToOrientationRotator(), AimingRotation).Yaw);
}

// ctrl+k & 0 折叠
// ctrl+k & l 展开
EMovementDirection UACSAnimInstance::CalculateQuadrant(EMovementDirection Current, float FR_Threshold,
	float FL_Threshold, float BR_Threshold, float BL_Threshold,
	float Buffer, float Angle)
{
	// UE_LOG(LogTemp, Warning, TEXT("%f"),Angle);
	if(AngleInRange(Angle, FL_Threshold, FR_Threshold, Buffer, Current != EMovementDirection::EMD_Forward))
	{
		return EMovementDirection::EMD_Forward;
	}
	if (AngleInRange(Angle, BL_Threshold, FL_Threshold, Buffer, Current != EMovementDirection::EMD_Left))
	{
		return EMovementDirection::EMD_Left;
	}
	if (AngleInRange(Angle, FR_Threshold, BR_Threshold, Buffer, Current != EMovementDirection::EMD_Right))
	{
		return EMovementDirection::EMD_Right;
	}
	return EMovementDirection::EMD_Backward;
}

bool UACSAnimInstance::AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool bIncreaseBuffer)
{
	////////////////////////////////////////////////////// 第五次更新

	const int32 sign = bIncreaseBuffer ? -1 : 1;
	return FMath::IsWithinInclusive(Angle, MinAngle + sign * Buffer, MaxAngle - sign * Buffer);

	////////////////////////////////////////////////////// 
}

/**
 * @brief 获得曲线值再加上 Bias 偏差值，放到 [ClampMin, ClampMax]中，超出区间就取端点，否则就是原来的值。
 */
float UACSAnimInstance::GetAnimCurveClamped(FName Name, float Bias, float ClampMin, float ClampMax) const
{
	return FMath::Clamp(GetCurveValue(Name) + Bias, ClampMin, ClampMax);
}

// 原地旋转检查
void UACSAnimInstance::TurnInPlaceCheck()
{
	// 角度足够大
	if (FMath::Abs(AimingAngle.X) > TurnCheckMinAngle)
	{
		ElapsedDelayTime += DeltaTimeX;
		// 停顿时间
		if(ElapsedDelayTime > 0.3f)
		{
			// 开始转向
			TurnInPlace(FRotator(0.f, AimingRotation.Yaw, 0.f), 1.f, 0.f, false);
		}
	}
	else
	{
		ElapsedDelayTime = 0.0f;
	}
}
void UACSAnimInstance::TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool bOverrideCurrent)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 第四次更新

	// 步骤1：设置转弯角度
	const float TurnAngle = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, Character->GetActorRotation()).Yaw;
	FTurnInPlace_Asset TargetTurnAsset;

	// UE_LOG(LogTemp, Warning, TEXT("%f"), TurnAngle);
	// 步骤2：根据转弯角度和站姿选择转弯资源
	if (FMath::Abs(TurnAngle) < Turn180Threshold)
	{
		if (TurnAngle < 0.f)
		{
			switch (Stance)
			{
			case EALS_Stance::EAS_Standing:
				TargetTurnAsset = N_TurnIP_L90;
				break;
			case EALS_Stance::EAS_Crouching:
				TargetTurnAsset = CLF_TurnIP_L90;
				break;
			}
		}
		else
		{
			switch (Stance)
			{
			case EALS_Stance::EAS_Standing:
				TargetTurnAsset = N_TurnIP_R90;
				break;
			case EALS_Stance::EAS_Crouching:
				TargetTurnAsset = CLF_TurnIP_R90;
				break;
			}
		}
	}
	else
	{
		if (TurnAngle < 0.f)
		{
			switch (Stance)
			{
			case EALS_Stance::EAS_Standing:
				TargetTurnAsset = N_TurnIP_L180;
				break;
			case EALS_Stance::EAS_Crouching:
				TargetTurnAsset = CLF_TurnIP_L180;
				break;
			}
		}
		else
		{
			switch (Stance)
			{
			case EALS_Stance::EAS_Standing:
				TargetTurnAsset = N_TurnIP_R180;
				break;
			case EALS_Stance::EAS_Crouching:
				TargetTurnAsset = CLF_TurnIP_R180;
				break;
			}
		}
	}
	
	// PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName, 0.2, 0.2,
								  // TargetTurnAsset.PlayRate * PlayRateScale, 1, 0.f, StartTime);

	// RotationScale = TurnAngle / TargetTurnAsset.AnimatedAngle * TargetTurnAsset.PlayRate * PlayRateScale;
	//步骤3：如果目标转弯动画未播放或设置为覆盖，则将转弯动画作为动态蒙太奇播放。
	if (bOverrideCurrent || !IsPlayingSlotAnimation(TargetTurnAsset.Animation, TargetTurnAsset.SlotName))
	{
		PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName, .2f, .2f,
		                                  TargetTurnAsset.PlayRate * PlayRateScale, 1, 0.f, StartTime);
	
		// 步骤4： 计算旋转的缩放量 缩放量无单位，只是旋转值的倍数关系。
		if (TargetTurnAsset.bScaleTurnAngle)
		{
			// 旋转值的缩放量  = 要旋转的角度 ÷ 动画播放的角度 × 旋转速率 × 动画播放速率缩放值
			//                   =          角度比例 × 旋转速率 × 动画播放速率缩放值
			RotationScale = TurnAngle / TargetTurnAsset.AnimatedAngle * TargetTurnAsset.PlayRate * PlayRateScale;
		}
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("开启"));
			// 因为这个状态没有开启任意角度旋转，而是使用动画原本的角度，所以角度比例就是 1
			// 所以 旋转的缩放量  = 旋转速率 × 动画播放速率缩放值
			RotationScale = TargetTurnAsset.PlayRate * PlayRateScale;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

// Lean插值
FLeanAmount UACSAnimInstance::InterpLeanAmount(FLeanAmount Current,FLeanAmount Target,float DeltaTime,float InterpSpeed)
{
	return FLeanAmount(FMath::FInterpTo(Current.LR,Target.LR,DeltaTime,InterpSpeed),
		FMath::FInterpTo(Current.FB,Target.FB,DeltaTime,InterpSpeed));
}

// 跳跃的接口，触发跳跃时调用(通过IALS_Animation接口)
void UACSAnimInstance::API_SetGroundedEntryState_Implementation(EGroundedEntryState m_GroundedEntryState)
{
	
}
void UACSAnimInstance::API_Jumped_Implementation()
{
	bJumped = true;

	// 一段时间后关闭
	GetWorld()->GetTimerManager().SetTimer(TH_Jumped,this,&UACSAnimInstance::ResetJumped,.1f);
}
void UACSAnimInstance::ResetJumped()
{
	bJumped = false;
}

float UACSAnimInstance::CalculateLandPrediction()
{
	return 0.0f;
}

FLeanAmount UACSAnimInstance::CalculateInAirLeanAmount() const
{
	// 获取相对于角色的局部速度， 再除以 350 得到一个比例值。 普通状态中， 走速：150 跑速：350 冲刺速：600，这里以跑速作为基准。
	const FVector Vector = Character->GetActorRotation().UnrotateVector(Velocity) / 350.f;

	// Y是代表的左右， X是代表的前后，所以值的位置需要换一下， 然后乘以下落倾斜度的比例。
	// 乘以曲线值是为了让来回倾斜更加丝滑。
	const FVector2D Vector2D = FVector2D(Vector.Y, Vector.X) * LeanInAirCurveFloat->GetFloatValue(FallSpeed);
	return {Vector2D.X, Vector2D.Y};
}