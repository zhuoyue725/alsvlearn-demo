// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "ALSVLearnDemoCharacter.h"
#include "ALSVLearnDemo/Data/Enums/ALS_RotationMode.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UALSAnimInstance::UALSAnimInstance()
{
	AnimatedWalkSpeed = 150.f;
	AnimatedRunSpeed = 350.f;
	AnimatedSprintSpeed = 600.f;

	AnimatedCrouchSpeed = 150.f;

	// 初始化为 12.f
	VelocityBlendInterpSpeed = 12.f;

	TurnCheckMinAngle = 45.f;

	AimYawRateLimit = 50.f;

	MinAngleDelay = 0.75f;
	MaxAngleDelay = 0.f;

	Turn180Threshold = 130.f;

	SmoothedAimingRotationInterpSpeed = 10.f;

	GroundedLeanInterpSpeed = 4.f;

	InAirLeanInterpSpeed = 4.f;
}

void UALSAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// 获取到角色指针
	Character = Cast<AALSVLearnDemoCharacter>(TryGetPawnOwner());
}

void UALSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	// 更新世界增量值
	DeltaTimeX = DeltaSeconds;

	// 如果没有获取到角色，直接停止
	if (DeltaTimeX == 0.f || !Character) return;

	// 更新角色相关信息
	UpdateCharacterInfo();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 第四次更新

	// 更新镜头瞄准、旋转相关变量
	UpdateAimingValues();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////// 第八次更新
	switch (MovementState)
	{
		bool bTemp;
	case EALS_MovementState::EAMs_None:
		break;
	case EALS_MovementState::EAMs_Grounded:

		/////////////////////////////////////////////////// 第四次更新
		// 判断是否在移动
		bTemp = ShouldMoveCheck();

		if (bTemp)
		{
			// 如果在运动，而且是刚开始运动的话，那么就将经过的延迟时间置空
			if (bTemp != bShouldMove)
			{
				bShouldMove = bTemp;
			
				// Changed to True
				ElapsedDelayTime = 0.f;
				// bRotateL = bRotateR = false;
			}

			// 在运动的话就要更新在运动状态下相关的值
			// While True
			UpdateMovementValues();

			UpdateRotationValues();
		}
		else
		{
			// 如果是刚开始停止， 对于当前来说没有具体的逻辑功能，将状态更新一下即可。
			if (bTemp != bShouldMove)
			{
				bShouldMove = bTemp;
			
				// Changed to False
			}

			// 如果是处于静止状态，判断是否可以原地旋转
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
		///////////////////////////////////////////////////

	case EALS_MovementState::EAMs_InAir:

		/////////////////////////////////////////////////// 第八次更新

		// 只有人物在下落状态的时候才会更新相关的值
		UpdateInAirValues();

		/////////////////////////////////////////////////// 

		break;
	case EALS_MovementState::EAMs_Mantling:
		break;
	case EALS_MovementState::EAMs_Ragdoll:
		break;
	}

	////////////////////////////////////////

}

void UALSAnimInstance::UpdateMovementValues()
{
	// 获取各个方向下帧的混合速度
	VelocityBlend = InterpVelocityBlend(VelocityBlend, CalculateVelocityBlend(), VelocityBlendInterpSpeed, DeltaTimeX);

	// TODO Set the Diagnal Scale Amount.

	////////////////////////////////////////////////////////////////////  第六次更新

	// 计算相对加速度
	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();

	// 计算身体倾斜度
	LeanAmount = InterpLeanAmount(LeanAmount, FLeanAmount(RelativeAccelerationAmount.Y, RelativeAccelerationAmount.X),
	                              GroundedLeanInterpSpeed, DeltaTimeX);

	////////////////////////////////////////////////////////////////////  

	// 计算是行走还是跑步
	WalkRunBlend = CalculateWalkRunBlend();

	// 计算步幅
	StrideBlend = CalculateStrideBlend();

	// 更新站立状态下的播放速率
	StandingPlayRate = CalculateStandingPlayRate();

	/////////////////////////////////////////////////////////////////// 第七次更新

	// 更新蹲伏状态下的播放速率
	CrouchingPlayRate = CalculateCrouchingPlayRate();

	///////////////////////////////////////////////////////////////////
}

void UALSAnimInstance::UpdateCharacterInfo()
{
	const IALS_Character* Character_API = Cast<IALS_Character>(Character);

	// 更新角色运动数值
	Character_API->Execute_API_GetEssentialValues(Character, Velocity, Acceleration, bIsMoving, bHasMovementInput,
	                                              Speed, AimingRotation, AimYawRate);

	// 更新人物运动状态
	Character_API->Execute_API_GetCurrentStates(Character, MovementState, MovementAction, Gait, Stance, RotationMode);
}

FVelocityBlend UALSAnimInstance::CalculateVelocityBlend()
{
	// 获取速度方向相对于角色来说反方向旋转【GetActorRotation()】的向量值
	const FVector LocRelativeVelocityDir = Character->GetActorRotation().UnrotateVector(Velocity.GetSafeNormal(.1f));
	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) + FMath::Abs(
		LocRelativeVelocityDir.Z);

	// 计算出各个方向所占的百分比 
	const FVector RelativeDirection = LocRelativeVelocityDir / Sum;

	return FVelocityBlend(
		FMath::Clamp(RelativeDirection.X, 0.f, 1.f), // 向前运动速度百分比
		FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.f, 0.f)), // 向后运动速度百分比
		FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.f, 0.f)), // 向左运动速度百分比
		FMath::Clamp(RelativeDirection.Y, 0.f, 1.f)); // 向右运动速度百分比
}

// 各个方向速度的差值计算
FVelocityBlend UALSAnimInstance::InterpVelocityBlend(FVelocityBlend Current, FVelocityBlend Target, float InterpSpeed,
                                                     float DeltaTime) const
{
	return FVelocityBlend(
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
float UALSAnimInstance::CalculateStrideBlend() const
{
	// 获得当前速度下向前走路的值
	const float WalkValue = StrideBlend_N_Walk->GetFloatValue(Speed);
	// 获得当前速度下向前跑步的值
	const float RunValue = StrideBlend_N_Run->GetFloatValue(Speed);

	/* 基于Alpha 计算 WalkValue 和 RunValue 之间的差值
	 * GetAnimCurveClamped(FName(TEXT("Weight_Gait")), -1.f, 0.f, 1.f) ： 获取当前运动动画的 Weight_Gait 曲线值，
	 * Weight_Gait ： 冲刺的时候为3， 跑步的时候为2， 走路的时候为1。
	 * 所以 Alpha ： 冲刺、跑步的时候为1， 走路的时候为0。
	 * 当Alpha = 1的时候返回RunValue值的100%
	 * 当Alpha = 0的时候返回WalkValue值的100%
	 * 就是根据当前状态返回步幅
	 */
	const float GaitValue = UKismetMathLibrary::Lerp(WalkValue, RunValue,
	                                                 GetAnimCurveClamped(FName(TEXT("Weight_Gait")), -1.f, 0.f, 1.f));

	///////////////////////////////////////////////////////////////////////// 第七次更新

	const float CLFValue = StrideBlend_C_Walk->GetFloatValue(Speed);
	return UKismetMathLibrary::Lerp(GaitValue, CLFValue, GetCurveValue(TEXT("BasePose_CLF")));

	///////////////////////////////////////////////////////////////////////// 
}

float UALSAnimInstance::CalculateLandPrediction()
{
	///////////////////////////////////////////////////////////////////////// 第八次更新

	/*
	 * 通过沿速度方向追踪，找到角色下落的可行走表面，
	 * 并获得“时间”（范围为0-1，1为最大值，0为即将着陆）直至碰撞，计算着陆预测权重。
	 * 土地预测曲线用于控制时间如何影响平滑混合的最终权重。
	 */

	// 我们后面要使用GetUnsafeNormal()，只用这个去标准化向量是比较快的，但是要保证向量一定不是零向量，所以这里就做一些处理。
	if (FallSpeed >= -200.f)
	{
		return 0.f;
	}

	// 获取角色的胶囊体
	const auto CapsuleComp = Character->GetCapsuleComponent();

	/*
	 * 检测的起始向量就是胶囊体的位置
	 * 终止向量是以速度为方向，大小下落速度值映射为[50,2000]的向量。
	 */
	const FVector StartLocation = CapsuleComp->GetComponentLocation();
	const FVector EndLocation = StartLocation + FVector(Velocity.X, Velocity.Y,
	                                                    FMath::Clamp(Velocity.Z, -4000.f, -200.f)).
		GetUnsafeNormal() * FMath::GetMappedRangeValueClamped(FVector2D(0.f, -4000.f), FVector2D(50.f, 2000.f),
		                                                      Velocity.Z);

	/*
	 * 就是生成一个与角色运动倾斜程度相同的胶囊体，来判断角色是否已经接触到地面了。
	 */
	FHitResult OutHit;
	UKismetSystemLibrary::CapsuleTraceSingleByProfile(GetWorld(), StartLocation, EndLocation,
	                                                  CapsuleComp->GetScaledCapsuleRadius(),
	                                                  CapsuleComp->GetScaledCapsuleHalfHeight(), TEXT("ALS_Character"),
	                                                  false, {}, EDrawDebugTrace::None, OutHit, true);

	// 判断是否接触到了的物体，并且是可行走的。
	if (OutHit.bBlockingHit && Character->GetCharacterMovement()->IsWalkable(OutHit))
	{
		/*
		 * OutHit.Time 表示的是接触位置在起始点和终止点的位置比例值。
		 * 如果在StartLocation处就已经接触了，那就是0。
		 * 如果值是在EndLocation处接触了，那就是1。
		 * 在中间接触了就按比例进行计算。
		 *
		 * 因为这个Time是由程序产生的，为了让他能够更加符合动画的融合度，所以就加入了曲线去控制他的这个值。
		 * Mask_LandPrediction 值为 0是完全使用曲线值，1的时候关闭曲线的值。
		 */
		return UKismetMathLibrary::Lerp(LandPredictionCurveFloat->GetFloatValue(OutHit.Time), 0.f,
		                                GetCurveValue(TEXT("Mask_LandPrediction")));
	}

	return 0.f;
	///////////////////////////////////////////////////////////////////////// 
}

FLeanAmount UALSAnimInstance::CalculateInAirLeanAmount() const
{
	/////////////////////////////////////////////////////////////////////////  第八次更新

	// 获取相对于角色的局部速度， 再除以 350 得到一个比例值。 普通状态中， 走速：150 跑速：350 冲刺速：600，这里以跑速作为基准。
	const FVector Vector = Character->GetActorRotation().UnrotateVector(Velocity) / 350.f;

	// Y是代表的左右， X是代表的前后，所以值的位置需要换一下， 然后乘以下落倾斜度的比例。
	// 乘以曲线值是为了让来回倾斜更加丝滑。
	const FVector2D Vector2D = FVector2D(Vector.Y, Vector.X) * LeanInAirCurveFloat->GetFloatValue(FallSpeed);
	return {Vector2D.X, Vector2D.Y};

	/////////////////////////////////////////////////////////////////////////  
}

bool UALSAnimInstance::ShouldMoveCheck() const
{
	return bIsMoving && bHasMovementInput || Speed > 150.f;
}

bool UALSAnimInstance::CanTurnInPlace() const
{
	// 人物曲线 “Enable_Transition” 判断是否可以进行动作过渡
	// TODO 还有两个条件没有加上去
	return GetCurveValue(TEXT("Enable_Transition")) > 0.99f;
}

void UALSAnimInstance::TurnInPlaceCheck()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 第四次更新

	//步骤1：检查瞄准角度是否在转弯检查最小角度之外，以及目标横摆角速度是否低于目标横摆角速度限制， 避免旋转太快而导致动作矛盾。
	// 如果是这样，开始计算经过的延迟时间。如果没有，则重置经过的延迟时间。这确保了在转向到位之前，不会再一次产生同样的动作。
	if (FMath::Abs(AimingAngle.X) > TurnCheckMinAngle && AimYawRate < AimYawRateLimit)
	{
		// UE_LOG(LogTemp, Warning, TEXT("%f"), TurnCheckMinAngle);
		ElapsedDelayTime += DeltaTimeX;
	
		/*
		 *步骤2：检查经过的延迟时间是否超过设置的延迟（映射到转角范围）。如果是这样，在适当位置触发转弯。
		 *原地旋转前有一定的时间，这个部分就是将让他旋转之间经过一些时间，时间就是角度映射到延迟的值。
		 */
		// UE_LOG(LogTemp, Warning, TEXT("%f"), FMath::Abs(AimingAngle.X));
	
		if (ElapsedDelayTime > FMath::GetMappedRangeValueClamped(FVector2D(TurnCheckMinAngle, 180.f),
		                                                         FVector2D(MinAngleDelay, MaxAngleDelay),
		                                                         FMath::Abs(AimingAngle.X)))
		{
			TurnInPlace(FRotator(0.f, AimingRotation.Yaw, 0.f), 1.f, 0.f, false);
		}
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("12312"));
		ElapsedDelayTime = 0.f;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 改写简单的
	// if (FMath::Abs(AimingAngle.X) > TurnCheckMinAngle)
	// {
	// 	ElapsedDelayTime += DeltaTimeX;
	// 	// 停顿时间
	// 	if(ElapsedDelayTime > 0.3f)
	// 	{
	// 		// 开始转向
	// 		TurnInPlace(FRotator(0.f, AimingRotation.Yaw, 0.f), 1.f, 0.f, false);
	// 	}
	// }
	// else
	// {
	// 	ElapsedDelayTime = 0.0f;
	// }
}

void UALSAnimInstance::TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool bOverrideCurrent)
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
	
	PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName, .2f, .2f,
								  TargetTurnAsset.PlayRate * PlayRateScale, 1, 0.f, StartTime);

	// 旋转缩放比例
	RotationScale = TurnAngle / TargetTurnAsset.AnimatedAngle * TargetTurnAsset.PlayRate * PlayRateScale;
	//步骤3：如果目标转弯动画未播放或设置为覆盖，则将转弯动画作为动态蒙太奇播放。
	// if (bOverrideCurrent || !IsPlayingSlotAnimation(TargetTurnAsset.Animation, TargetTurnAsset.SlotName))
	// {
	// 	PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName, .2f, .2f,
	// 	                                  TargetTurnAsset.PlayRate * PlayRateScale, 1, 0.f, StartTime);
	//
	// 	// 步骤4： 计算旋转的缩放量 缩放量无单位，只是旋转值的倍数关系。
	// 	if (TargetTurnAsset.bScaleTurnAngle)
	// 	{
	// 		// 旋转值的缩放量  = 要旋转的角度 ÷ 动画播放的角度 × 旋转速率 × 动画播放速率缩放值
	// 		//                   =          角度比例 × 旋转速率 × 动画播放速率缩放值
	// 		RotationScale = TurnAngle / TargetTurnAsset.AnimatedAngle * TargetTurnAsset.PlayRate * PlayRateScale;
	// 	}
	// 	else
	// 	{
	// 		// UE_LOG(LogTemp, Warning, TEXT("开启"));
	// 		// 因为这个状态没有开启任意角度旋转，而是使用动画原本的角度，所以角度比例就是 1
	// 		// 所以 旋转的缩放量  = 旋转速率 × 动画播放速率缩放值
	// 		RotationScale = TargetTurnAsset.PlayRate * PlayRateScale;
	// 	}
	// }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void UALSAnimInstance::DynamicTransitionCheck()
{
	if (UKismetAnimationLibrary::K2_DistanceBetweenTwoSocketsAndMapRange(
		GetOwningComponent(), TEXT("ik_foot_l"), RTS_Component, TEXT("VB foot_target_l"), RTS_Component,
		false, 0.f, 0.f, 0.f, 0.f) > 8.f)
	{
	}
}

void UALSAnimInstance::PlayDynamicTransition(float ReTriggerDelay, FDynamicMontageParams Parameters)
{
}


void UALSAnimInstance::UpdateAimingValues()
{
	// 差值计算 Aiming Rotation 值以实现平滑的瞄准旋转变化。在计算角度之前对旋转进行插值可确保该值不受演员旋转变化的影响，允许慢速瞄准旋转变化与快速演员旋转变化同时进行。
	SmoothedAimingRotation = FMath::RInterpTo(SmoothedAimingRotation, AimingRotation, DeltaTimeX,
	                                          SmoothedAimingRotationInterpSpeed);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 第四次更新

	// 通过获得瞄准旋转和Actor旋转之间的增量，计算瞄准角度和平滑瞄准角度。
	FRotator Rotation = UKismetMathLibrary::NormalizedDeltaRotator(AimingRotation, Character->GetActorRotation());
	AimingAngle = FVector2D(Rotation.Yaw, Rotation.Pitch);
	//
	// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	Rotation = UKismetMathLibrary::NormalizedDeltaRotator(SmoothedAimingRotation, Character->GetActorRotation());
	SmoothedAimingAngle = FVector2D(Rotation.Yaw, Rotation.Pitch);
	
	// TODO 这里有个判断
	
	AimSweepTime = FMath::GetMappedRangeValueClamped(FVector2D(-90.f, 90.f), FVector2D(1.f, 0.f), AimingAngle.Y);
	
	SpineRotation.Yaw = AimingAngle.X / 4.f;

	// TODO 后面有两个逻辑还没有写
}

void UALSAnimInstance::UpdateRotationValues()
{
	////////////////////////////////////////////////////// 第五次更新

	MovementDirection = CalculateMovementDirection();

	//////////////////////////////////////////////////////

	const float Time = UKismetMathLibrary::NormalizedDeltaRotator(Velocity.ToOrientationRotator(),
	                                                              Character->GetControlRotation()).Yaw;
	FVector CurveValue = YawOffset_FB->GetVectorValue(Time);
	FYaw = CurveValue.X;
	BYaw = CurveValue.Y;

	CurveValue = YawOffset_LR->GetVectorValue(Time);
	LYaw = CurveValue.X;
	RYaw = CurveValue.Y;
}

void UALSAnimInstance::UpdateInAirValues()
{
	///////////////////////////////////// 第八次更新

	// 下落速度就是人物运动在Z轴的速度，一般是 <= 0。
	FallSpeed = Velocity.Z;

	LandPrediction = CalculateLandPrediction();

	LeanAmount = InterpLeanAmount(LeanAmount, CalculateInAirLeanAmount(), InAirLeanInterpSpeed, DeltaTimeX);

	///////////////////////////////////// 
}

EMovementDirection UALSAnimInstance::CalculateMovementDirection()
{
	////////////////////////////////////////////////////// 第五次更新

	// 冲刺状态就只能向前冲， 所以需要提前预判一下。
	if (Gait == EALS_Gait::EAG_Sprinting) return EMovementDirection::EMD_Forward;

	// TODO Rotation Mode

	/*
	 * 关于传参角度的计算
	 * 速度方向所对应的角度并不是完全等于我们想要他移动到的那个方向所对应的角度。
	 * 角色在运动的时候会伴随着控制的移动而移动（保证人物朝向和镜头一致），那么移动的时候速度的Yaw值实际上是加上了控制器的Yaw值。
	 * 所以需要让速度所对应的角度减去控制器所对应的角度。
	 */
	return CalculateQuadrant(MovementDirection, 70.f, -70.f, 110.f, -110.f, 5.f,
	                         UKismetMathLibrary::NormalizedDeltaRotator(Velocity.ToOrientationRotator(), AimingRotation)
	                         .Yaw);

	//////////////////////////////////////////////////////
}

EMovementDirection UALSAnimInstance::CalculateQuadrant(EMovementDirection Current, float FR_Threshold,
                                                       float FL_Threshold, float BR_Threshold, float BL_Threshold,
                                                       float Buffer, float Angle)
{
	////////////////////////////////////////////////////// 第五次更新

	/*
	 * 如果角度所对应的方向是和目前方向一致的话就缩小范围，否则就扩大范围，目的就是为了增加一点缓冲值。
	 * 缩小、扩大范围能够更快的像其他方向过渡。
	 */

	if (AngleInRange(Angle, FL_Threshold, FR_Threshold, Buffer, Current != EMovementDirection::EMD_Forward))
	{
		return EMovementDirection::EMD_Forward;
	}

	if (AngleInRange(Angle, FR_Threshold, BR_Threshold, Buffer, Current != EMovementDirection::EMD_Right))
	{
		return EMovementDirection::EMD_Right;
	}

	if (AngleInRange(Angle, BL_Threshold, FL_Threshold, Buffer, Current != EMovementDirection::EMD_Right))
	{
		return EMovementDirection::EMD_Left;
	}

	return EMovementDirection::EMD_Backward;

	////////////////////////////////////////////////////// 
}

bool UALSAnimInstance::AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool bIncreaseBuffer)
{
	////////////////////////////////////////////////////// 第五次更新

	const int32 sign = bIncreaseBuffer ? -1 : 1;
	return FMath::IsWithinInclusive(Angle, MinAngle + sign * Buffer, MaxAngle - sign * Buffer);

	////////////////////////////////////////////////////// 
}

/**
 * @brief  先获取最大 加/减 速度， 将目前的加速度限制在最大 加/减 速度中，再除以醉倒 最大 加/减 速度，将加速度限制在 0 ~ 1范围内，
 * 使用使用 UnrotateVector 转化为局部坐标系那么他就会被归一化到-1到1的范围，这样-1等于最大制动减速，1等于角色移动组件的最大加速度。
 */
FVector UALSAnimInstance::CalculateRelativeAccelerationAmount()
{
	float MaxValue;
	/*
	 * 两个向量进行点积
	 * a · b > 0 => 基本上是同一个方向
	 * a · b = 0 => 两个方向垂直
	 * a · b < 0 => 基本上是两个不同的方向
	*/
	if (FVector::DotProduct(Acceleration, Velocity) > 0.f)
	{
		// 如果加速度方向和速度方向是同一个方向，那么值就选择最大加速度。
		MaxValue = Character->GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{
		// 如果加速度方向和速度方向不是同一个方向，那么值就选择最大减速度。
		MaxValue = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();
	}

	// 使用 UnrotateVector 转化为局部坐标系， 那么当前的值就是表示为相对于actor旋转的当前加速度/减速量
	return Character->GetActorRotation().UnrotateVector(Acceleration.GetClampedToMaxSize(MaxValue) / MaxValue); // Acceleration归一化，转换到Actor的本地空间
}

float UALSAnimInstance::CalculateWalkRunBlend()
{
	if (Gait == EALS_Gait::EAG_Walking)
	{
		return 0.f;
	}
	return 1.f;
}

float UALSAnimInstance::CalculateStandingPlayRate()
{
	////////////////////////////////////////////   第三次更新内容

	// 设置播放速率，目的是为了让动画匹配上速度, 粗略的计算公式为 ： 播放速率 = 当前速度 / 动画速度
	// 曲线weight_Gait 的取值区间范围为[1,3] 分别代表 走 跑 冲

	/*
	 * V 的取值范围为： [0, 1] 意思就走路的时候 V = 0.f, 跑或者冲的时候 V = 1.f。
	 * 因为 分子的值都是一样的, 分母越大，值就越小，所以 A > B 
	 * 根据Lerp的定义可得，走路的时候V = 0.f, 那么采用的就是 走路播放速率，完全跑步的时候V = 1.f, 采用的是 跑步的播放速率。
	 */
	const float AnimationSpeed = UKismetMathLibrary::Lerp(Speed / AnimatedWalkSpeed, Speed / AnimatedRunSpeed,
	                                                      GetAnimCurveClamped(TEXT("Weight_Gait"), -1.f, 0.f, 1.f));
	/*
	 * V 的取值范围为： [-1, 1] 意思就走、跑的时候 V = 0.f, 冲的时候 V = 1.f。
	 */
	const float AnimationSprintSpeed = UKismetMathLibrary::Lerp(AnimationSpeed, Speed / AnimatedSprintSpeed,
	                                                            GetAnimCurveClamped(
		                                                            TEXT("Weight_Gait"), -2.f, 0.f, 1.f));
	// 根据以上代码分析可得出，这两行代码的主要的意思就是利用插值将合适的动画播放速率挑选出来。


	// 根据提供的数据，最大播放速率有 4点多倍。
	// 但是还有一些情况没有考虑到， 比如说步幅和人物的缩放比例，步幅变小或者缩放比例变小的时候，动画的播放速率是要适当的提高。
	return FMath::Clamp(AnimationSprintSpeed / StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.f, 3.f);
}

float UALSAnimInstance::CalculateCrouchingPlayRate()
{
	return FMath::Clamp(Speed / AnimatedCrouchSpeed / StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.f,
	                    2.f);
}

FLeanAmount UALSAnimInstance::InterpLeanAmount(FLeanAmount Current, FLeanAmount Target, float InterpSpeed,
                                               float DeltaTime)
{
	return FLeanAmount(FMath::FInterpTo(Current.LR, Target.LR, DeltaTime, InterpSpeed),
	                   FMath::FInterpTo(Current.FB, Target.FB, DeltaTime, InterpSpeed));
}

void UALSAnimInstance::PlayTransition(FDynamicMontageParams Parameters)
{
	PlaySlotAnimationAsDynamicMontage(Parameters.Animation, TEXT("Grounded Slot"), Parameters.BlendInTime,
	                                  Parameters.BlendOutTime, Parameters.PlayRate, 1, 0.f, Parameters.StartTime);
}

void UALSAnimInstance::API_SetGroundedEntryState_Implementation(EGroundedEntryState m_GroundedEntryState)
{
	GroundedEntryState = m_GroundedEntryState;
}

void UALSAnimInstance::API_Jumped_Implementation()
{
	/////////////////////////////////////// 第八次更新

	/*
	 * 只要触发了这个函数，就说明角色在跳跃状态，先将bJumped设置为True, 过一段时间之后将其恢复成false。
	 */
	bJumped = true;

	// 将水平速度放到[0,600]的范围中，输出一个在[1.2,1.5]区间中的值作为跳跃播放速度
	JumpPlayRate = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 600.f), FVector2D(1.2f, 1.5f), Speed);

	GetWorld()->GetTimerManager().SetTimer(TH_Jumped, this, &UALSAnimInstance::ResetJumped, .1f);

	///////////////////////////////////////  
}


void UALSAnimInstance::ResetJumped()
{
	/////////////////////////////////////// 第八次更新

	bJumped = false;

	/////////////////////////////////////// 
}

/**
 * @brief 获得曲线值再加上 Bias 偏差值，放到 [ClampMin, ClampMax]中，超出区间就取端点，否则就是原来的值。
 */
float UALSAnimInstance::GetAnimCurveClamped(FName Name, float Bias, float ClampMin, float ClampMax) const
{
	return FMath::Clamp(GetCurveValue(Name) + Bias, ClampMin, ClampMax);
}
