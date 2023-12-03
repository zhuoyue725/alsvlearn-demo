// Copyright Epic Games, Inc. All Rights Reserved.

#include "ALSVLearnDemoCharacter.h"

#include "ALSVLearnDemo/Data/Enums/ALS_RotationMode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

//////////////////////////////////////////////////////////////////////////
// AALSVLearnDemoCharacter

AALSVLearnDemoCharacter::AALSVLearnDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	// GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 420.f;
	GetCharacterMovement()->AirControl = 0.15f;

	// 没设置一个变量一定要对他进行初始化，除非他不需要初始化。
	ThirdPersonFOV = 90.f;
	FirstPersonFOV = 90.f;

	LookUpRate = 1.f;
	LookRightRate = 1.f;

	////////////////////////////////////////////   第三次更新内容
	Gait = EALS_Gait::EAG_Walking;
	DesiredGait = EALS_Gait::EAG_Running;
	////////////////////////////////////////////

	////////////////////////////////////////////   第七次更新内容

	// 在 #include "AI/Navigation/NavigationTypes.h" 的 FMovementProperties 中 叫 bCanCrouch。
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	////////////////////////////////////////////
}

void AALSVLearnDemoCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnBeginPlay();
}

void AALSVLearnDemoCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	Stance = EALS_Stance::EAS_Crouching;
}

void AALSVLearnDemoCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	Stance = EALS_Stance::EAS_Standing;
}

//////////////////////////////////////////////////////////////////////////
// Input
// 这里一定要改一下， 要和复刻项目的那个按键名一模一样
void AALSVLearnDemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// 绑定跳跃按键
	// PlayerInputComponent->BindAction("JumpAction", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("JumpAction", IE_Pressed, this, &AALSVLearnDemoCharacter::JumpAction);
	PlayerInputComponent->BindAction("JumpAction", IE_Released, this, &ACharacter::StopJumping);

	// 绑定冲刺按键
	PlayerInputComponent->BindAction("SprintAction", IE_Pressed, this, &AALSVLearnDemoCharacter::MoveSprint_Pressed);
	PlayerInputComponent->BindAction("SprintAction", IE_Released, this, &AALSVLearnDemoCharacter::MoveSprint_Released);

	////////////////////////////////////////////   第三次更新内容

	// 绑定行走和跑步切换按键
	PlayerInputComponent->BindAction("WalkAction", IE_Pressed, this, &AALSVLearnDemoCharacter::MoveWalk);

	////////////////////////////////////////////

	////////////////////////////////////////////   第七次更新内容

	// 切换行走 、 蹲伏 、 滚动向前状态 
	PlayerInputComponent->BindAction("StanceAction", IE_Pressed, this, &AALSVLearnDemoCharacter::StanceAction);

	////////////////////////////////////////////


	// 绑定向前向后移动按键
	PlayerInputComponent->BindAxis("MoveForward/Backwards", this, &AALSVLearnDemoCharacter::MoveForward);

	// 绑定向左向右移动按键
	PlayerInputComponent->BindAxis("MoveRight/Left", this, &AALSVLearnDemoCharacter::MoveRight);

	// 绑定向左向右看按键
	PlayerInputComponent->BindAxis("LookLeft/Right", this, &AALSVLearnDemoCharacter::LookRight);

	// 绑定向上向下看按键
	PlayerInputComponent->BindAxis("LookUp/Down", this, &AALSVLearnDemoCharacter::LookUp);
}

bool AALSVLearnDemoCharacter::MantleCheck_Implementation()
{
	// 检测是否允许攀爬
	// 1 射线检测
	// 1.1 向前射线检测
	// 1.2 向下射线检测

	// 2.开始
	UE_LOG(LogTemp,Warning,TEXT("MantleCheck"));
	return false;
}
void AALSVLearnDemoCharacter::MantleUpdate_Implementation()
{
	
}
void AALSVLearnDemoCharacter::MantleStart_Implementation()
{
	
}

void AALSVLearnDemoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 更新角色运动相关变量
	SetEssentialValues();


	switch (MovementState)
	{
	case EALS_MovementState::EAMs_Grounded:

		////////////////////////////////////////////   第三次更新内容

		// 更新角色移动、走跑、冲刺
		UpdateCharacterMovement();

		////////////////////////////////////////////

		////////////////////////////////////////////   第四次更新内容

		UpdateGroundedRotation();

		break;
		////////////////////////////////////////////

	case EALS_MovementState::EAMs_InAir:

		////////////////////////////////////////////   第八次更新内容

		UpdateInAirRotation();
		break;
	default: ;

		////////////////////////////////////////////   
	}


	////////////////////////////////////////////   第四次更新内容

	CacheValues();
	////////////////////////////////////////////  
}

void AALSVLearnDemoCharacter::API_GetCurrentStates_Implementation(EALS_MovementState& Eals_MovementState,
                                                                  EALS_MovementAction& m_MovementAction,
                                                                  EALS_Gait& Eals_Gait, EALS_Stance& Eals_Stance,
                                                                  EALS_RotationMode& Eals_RotationMode)
{
	Eals_MovementState = MovementState;
	Eals_Gait = Gait;
	Eals_Stance = Stance;
	m_MovementAction = MovementAction;
	Eals_RotationMode = RotationMode;
}

void AALSVLearnDemoCharacter::CacheValues()
{
	////////////////////////////////////////////   第四次更新内容

	PreviousVelocity = GetVelocity();
	PreviousAimYaw = GetControlRotation().Yaw;
	////////////////////////////////////////////
}

void AALSVLearnDemoCharacter::API_GetEssentialValues_Implementation(FVector& m_Velocity, FVector& m_Acceleration,
                                                                    bool& m_bIsMoving,
                                                                    bool& m_bHasMovementInput, float& m_Speed,
                                                                    FRotator& m_AimingRotation, float& m_AimYawRate)
{
	m_Velocity = GetVelocity();
	m_bIsMoving = bIsMoving;
	m_bHasMovementInput = bHasMovementInput;
	m_Speed = Speed;
	m_AimingRotation = GetControlRotation();
	m_Acceleration = Acceleration;
	m_AimYawRate = AimYawRate;
}

void AALSVLearnDemoCharacter::SetEssentialValues()
{
	//////////////////////////////////////////////////// 第六次更新

	Acceleration = CalculateAcceleration();

	//////////////////////////////////////////////////// 

	// 因为我们人物在地面上运动只需要水平的速度值大小，我们获取到人物移动速度之后将Z轴去掉的大小就是当前在地面上运动的速度大小。
	Speed = FVector(GetVelocity().X, GetVelocity().Y, 0.f).Size();

	// 只要是速度大小大于1.f的都算作正在运动
	bIsMoving = Speed > 1.f;

	if (bIsMoving)
	{
		////////////////////////////////////////////   第四次更新内容

		LastVelocityRotator = GetVelocity().ToOrientationRotator();
		////////////////////////////////////////////
	}

	/*
	 * GetCurrentAcceleration ： 当前加速度矢量（带幅值）。 每次更新都会根据输入向量、MaxAcceleration和当前移动模式的约束来计算该值。
	 * 那么这个公式的意思就是只要我的加速度矢量值大小不为零，那就代表有运动按键输入
	 */
	MovementInputAmount = GetCharacterMovement()->GetCurrentAcceleration().Size() / GetCharacterMovement()->
		GetMaxAcceleration();
	bHasMovementInput = MovementInputAmount > 0.f;

	if (bHasMovementInput)
	{
		// TODO Set Last Movement Input Rotation
	}

	AimYawRate = FMath::Abs((GetControlRotation().Yaw - PreviousAimYaw) / GetWorld()->DeltaTimeSeconds);
}

void AALSVLearnDemoCharacter::OnBeginPlay()
{
	// 确保网格和animbp在CharacterBP之后更新，以确保其获得最新的值。
	GetMesh()->AddTickPrerequisiteActor(this);

	MainAnimInstance = GetMesh()->GetAnimInstance();

	SetMovementModel(TEXT("Normal"));

	TargetRotation = LastVelocityRotator = LastMovementInputRotation = GetActorRotation();
}

void AALSVLearnDemoCharacter::UpdateCharacterMovement()
{
	////////////////////////////////////////////   第三次更新内容，涉及到的函数变量均是

	const EALS_Gait AllowedGait = GetAllowedGait();
	const EALS_Gait ActualGait = GetActualGait(AllowedGait);

	if (Gait != ActualGait)
	{
		SetGait(ActualGait);
	}

	// 蓝图中的 Sequence 的每一个顺序是不受到前面情况的影响的
	// 比如说他上一个顺序的逻辑中有一个判断条件，是false，不会通过后面的逻辑，但是并不影响执行下一个序号的逻辑
	UpdateDynamicMovementSettings(AllowedGait);

	////////////////////////////////////////////  
}

UAnimMontage* AALSVLearnDemoCharacter::GetRollAnimation()
{
	return nullptr;
}

FMovementSettings AALSVLearnDemoCharacter::GetTargetMovementSettings()
{
	////////////////////////////////////////////   第三次更新内容

	// TODO 添加其他判断条件  目前只用得到一种数据，所以只写一种。
	return MovementData.VelocityDirection.Standing;

	////////////////////////////////////////////   第三次更新内容
}

EALS_Gait AALSVLearnDemoCharacter::GetActualGait(EALS_Gait AllowedGait)
{
	////////////////////////////////////////////   第三次更新内容

	// 速度已经超过了跑步的速度了
	if (Speed >= CurrentMovementSettings.RunSpeed + 10.f)
	{
		// 如果还是走路状态的话，就将他设置为跑步状态
		if (AllowedGait == EALS_Gait::EAG_Walking) return EALS_Gait::EAG_Running;
		// 是跑步还是冲刺，按照传进来的参数返回即可
		return AllowedGait;
	}

	// 已经超过了走路的速度，就设置为跑步状态，到这一步的时候冲刺条件已经不符合了。
	if (Speed >= CurrentMovementSettings.WalkSpeed + 10.f)
	{
		return EALS_Gait::EAG_Running;
	}

	// 剩下的就是走路状态了。
	return EALS_Gait::EAG_Walking;

	////////////////////////////////////////////  
}

void AALSVLearnDemoCharacter::SetMovementModel(FName RowName)
{
	////////////////////////////////////////////   第三次更新内容

	MovementData = *MovementModelDB->FindRow<FMovementSettingsState>(RowName, "");
	////////////////////////////////////////////  
}


void AALSVLearnDemoCharacter::UpdateDynamicMovementSettings(EALS_Gait AllowedGait)
{
	////////////////////////////////////////////   第三次更新内容

	CurrentMovementSettings = GetTargetMovementSettings();

	// 初始化最大运动数据
	switch (AllowedGait)
	{
	case EALS_Gait::EAG_Walking:
		GetCharacterMovement()->MaxWalkSpeed = CurrentMovementSettings.WalkSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CurrentMovementSettings.WalkSpeed;
		break;
	case EALS_Gait::EAG_Running:
		GetCharacterMovement()->MaxWalkSpeed = CurrentMovementSettings.RunSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CurrentMovementSettings.RunSpeed;
		break;
	case EALS_Gait::EAG_Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = CurrentMovementSettings.SprintSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CurrentMovementSettings.SprintSpeed;
		break;
	}

	const FVector CurveVector = CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed());
	// 最大加速度
	GetCharacterMovement()->MaxAcceleration = CurveVector.X;
	// 行走反作用力，给运动减速用的
	GetCharacterMovement()->BrakingDecelerationWalking = CurveVector.Y;
	// 地面摩擦力 影响运动控制的设置。值越高，方向变化越快。
	GetCharacterMovement()->GroundFriction = CurveVector.Z;

	//////////////////////////////////////////// 
}

float AALSVLearnDemoCharacter::GetMappedSpeed()
{
	////////////////////////////////////////////   第三次更新内容

	const float WalkSpeed = CurrentMovementSettings.WalkSpeed;
	const float RunSpeed = CurrentMovementSettings.RunSpeed;
	const float SprintSpeed = CurrentMovementSettings.SprintSpeed;

	// 根据当前速度返回一个对应曲线的一个值，
	// 0 - 停止速度 1 - 行走速度 2 - 跑步速度 3 - 冲刺速度
	// 如果在他们的中间，那么就返回这个速度在他们区间的一个百分比， GetMappedRangeValueClamped函数就实现了这个需求。
	// GetMappedRangeValueClamped 函数的作用就是将 Value 在 InputRange中取值对应的百分比的值给 OutputRange.X 到 OutputRange.Y 做插值计算。

	if (Speed > RunSpeed)
	{
		return FMath::GetMappedRangeValueClamped(FVector2D(RunSpeed, SprintSpeed), FVector2D(2.f, 3.f), Speed);
	}
	if (Speed > WalkSpeed)
	{
		return FMath::GetMappedRangeValueClamped(FVector2D(WalkSpeed, RunSpeed), FVector2D(1.f, 2.f), Speed);
	}

	return FMath::GetMappedRangeValueClamped(FVector2D(0.f, WalkSpeed), FVector2D(0.f, 1.f), Speed);

	////////////////////////////////////////////
}

EALS_Gait AALSVLearnDemoCharacter::GetAllowedGait()
{
	////////////////////////////////////////////   第三次更新内容

	// 已经按下冲刺键，但是不能冲刺的时候改成跑步状态，其他的都正常就行，因为其他的没有限制。
	if (DesiredGait == EALS_Gait::EAG_Sprinting && !CanSprint())
	{
		return EALS_Gait::EAG_Running;
	}
	return DesiredGait;

	//////////////////////////////////////////// 
}

bool AALSVLearnDemoCharacter::CanSprint()
{
	if (!bHasMovementInput) return false;

	// TODO 检测是否可以冲刺逻辑实现

	return true;
}

void AALSVLearnDemoCharacter::UpdateGroundedRotation()
{
	////////////////////////////////////////////   第四次更新内容

	if (CanUpdateMovingRotation())
	{
		// 速度方向旋转 TODO Velocity Direction Rotation
		if(RotationMode == EALS_RotationMode::EARm_VelocityDirection)
		{
			SmoothCharacterRotation(FRotator(0.f,LastVelocityRotator.Yaw,0),800.0f,CalculateGroundedRotationRate());
		}
		else if(RotationMode == EALS_RotationMode::EARm_LookingDirection)
		{
			////////////////////////////////////////////////////// 第五次更新

			if (Gait == EALS_Gait::EAG_Sprinting)
			{
				// 人物向上一个位置进行旋转，冲刺的时候因为速度比较快，所以旋转的要稍微慢一点
				SmoothCharacterRotation(FRotator(0.f, LastVelocityRotator.Yaw, 0.f), 500.f,
										CalculateGroundedRotationRate());
			}
			else
			{
				// 跑步、 走路状态就要加上当前的一点位置偏移，有一点点身体倾倒的感觉
				SmoothCharacterRotation(FRotator(0.f, GetControlRotation().Yaw + GetAnimCurveValue(TEXT("YawOffset")), 0.f),
										500.f, CalculateGroundedRotationRate());
			}
			////////////////////////////////////////////////////// 第五次更新	
		}
	}
	else // 原地旋转
	{
		// RotationAmount曲线中取出来的值CurrentSecondRotation代表的是当前时间(s)的旋转旋转差值。 可理解为 旋转度每秒
		const float CurrentSecondRotation = GetAnimCurveValue(TEXT("RotationAmount"));
		if (FMath::Abs(CurrentSecondRotation) > 0.001)
		{
			/*
			 * AddActorWorldRotation 就是在原基础上添加一个Rotation的值，这个添加是一次性的，也没有过渡的过程，一步到位。
			 * 如果没有乘倍数，旋转的值会很大，为什么？
			 * 因为没有乘以倍数的话，那么添加的值就会等于  1 s * CurrentSecondRotation。
			 * 实际上 GetWorld()->DeltaTimeSeconds是比1s小很多的，
			 * 再加上浮点计算有精度丢失的情况，那么就会导致最终累加起来的值和我们想要的值相差很大，所以需要乘以一个值来减少他的误差。
			 */
			AddActorWorldRotation(FRotator(0.f, GetWorld()->DeltaTimeSeconds * 30.f * CurrentSecondRotation, 0.f));
			TargetRotation = GetActorRotation();
		}
	}
	//////////////////////////////////////////// 
}

bool AALSVLearnDemoCharacter::CanUpdateMovingRotation()
{
	////////////////////////////////////////////   第四次更新内容

	return (bIsMoving && bHasMovementInput || Speed > 150.f) && !HasAnyRootMotion();
	//////////////////////////////////////////// 
}

void AALSVLearnDemoCharacter::SmoothCharacterRotation(FRotator Target, const float TargetInterpSpeed,
                                                      const float ActorInterpSpeed)
{
	////////////////////////////////////////////   第四次更新内容

	// 以恒定速率达到目标旋转。 TargetRotation 当前Actor旋转的值
	TargetRotation = FMath::RInterpConstantTo(TargetRotation, Target, GetWorld()->DeltaTimeSeconds, TargetInterpSpeed);
	// 尝试基于当前旋转达到目标旋转，在旋转到目标旋转时提供良好的平滑感觉。
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->DeltaTimeSeconds,
	                                              ActorInterpSpeed);
	SetActorRotation(NewRotation);
	////////////////////////////////////////////
}

void AALSVLearnDemoCharacter::UpdateInAirRotation()
{
	// TODO  Rotation Mode
	InAirRotation = TargetRotation;
	
	// 让角色平滑的旋转到我们想要的旋转值上。
	SmoothCharacterRotation(FRotator(0.f, InAirRotation.Yaw, 0.f), 0.f, 5.f);
}

float AALSVLearnDemoCharacter::CalculateGroundedRotationRate()
{
	// MapRangeClamped 返回从一个范围映射到另一个范围的值，其中该值被牵制到输入范围。（例如，从0->1到0->50范围标准化的0.5将导致25）
	return CurrentMovementSettings.RotationRateCurve->GetFloatValue(GetMappedSpeed()) *
		FMath::GetMappedRangeValueClamped(FVector2D(0.f, 300.f), FVector2D(1.f, 3.f), AimYawRate);
}

bool AALSVLearnDemoCharacter::GetCameraParameters_Implementation(float& TP_FOV, float& FP_FOV)
{
	TP_FOV = ThirdPersonFOV;
	FP_FOV = FirstPersonFOV;
	return bRightShoulder;
}

void AALSVLearnDemoCharacter::Get3PTraceParams_Implementation(FVector& TraceOrigin, float& TraceRadius,
                                                              TEnumAsByte<ETraceTypeQuery>& TraceChanel)
{
	TraceOrigin = GetActorLocation();
	TraceRadius = 10.0;
	TraceChanel = TraceTypeQuery1;
}

void AALSVLearnDemoCharacter::JumpAction()
{
	// 跳跃判断是否攀爬
	if(MovementState == EALS_MovementState::EAMs_Grounded)
	{
		if(bHasMovementInput)
		{
			if(!MantleCheck())
			{
				Jump();
			}
		}
		else
		{
			if(Stance == EALS_Stance::EAS_Crouching)
			{
				// 解除蹲伏
				UnCrouch();
			}
			else
			{
				Jump();
			}
		}
	}
	else if(MovementState == EALS_MovementState::EAMs_InAir)
	{
		MantleCheck();
	}
}

void AALSVLearnDemoCharacter::OnJumped_Implementation()
{
	//////////////////////////////////////////// 第八次更新

	// 当按下跳跃键的时候就会触发此函数(不用自行绑定也会触发)

	// 如果速度大于了100，那在空中的跳跃值就根据速度的旋转值来，否则就是角色的旋转值。
	InAirRotation = Speed > 100.f ? LastVelocityRotator : GetActorRotation();
	
	if (IsValid(MainAnimInstance))
	{
		IALS_Animation* AnimationInterface = Cast<IALS_Animation>(MainAnimInstance);
		if (!AnimationInterface) return;
		
		// 直接开始跳
		AnimationInterface->Execute_API_Jumped(MainAnimInstance);
	}

	//////////////////////////////////////////// 
}

void AALSVLearnDemoCharacter::API_SetMovementAction_Implementation(EALS_MovementAction Eals_MovementAction)
{
	//////////////////////////////////////////// 第八次更新

	if (MovementAction != Eals_MovementAction)
	{
		if (Eals_MovementAction == EALS_MovementAction::EAMs_Rolling)
		{
			Crouch();
		}

		if (MovementAction == EALS_MovementAction::EAMs_Rolling)
		{
			switch (DesiredStance)
			{
			case EALS_Stance::EAS_Standing:
				UnCrouch();
				break;
			case EALS_Stance::EAS_Crouching:
				Crouch();
				break;
			}
		}
		MovementAction = Eals_MovementAction;
	}

	//////////////////////////////////////////// 
}

void AALSVLearnDemoCharacter::SetGait(EALS_Gait NewGait)
{
	if (NewGait != Gait)
	{
		Gait = NewGait;
	}
}

void AALSVLearnDemoCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	////////////////////////////////////////////////////////// 第八次更新

	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	OnCharacterMovementModeChanged(PrevMovementMode, GetCharacterMovement()->MovementMode, PreviousCustomMode,
	                               GetCharacterMovement()->CustomMovementMode);

	////////////////////////////////////////////////////////// 
}

void AALSVLearnDemoCharacter::OnCharacterMovementModeChanged(EMovementMode PrevMovementMode,
                                                             EMovementMode NewMovementMode, uint8 PrevCustomMode,
                                                             uint8 NewCustomMode)
{
	////////////////////////////////////////////////////////// 第八次更新

	/*
	 * 根据 Character自带的运动模式来更新 MovementState
	 */
	if (NewMovementMode == MOVE_Walking || NewMovementMode == MOVE_NavWalking)
	{
		SetMovementState(EALS_MovementState::EAMs_Grounded);
	}
	if (NewMovementMode == MOVE_Falling)
	{
		SetMovementState(EALS_MovementState::EAMs_InAir);
	}

	////////////////////////////////////////////////////////// 
}

void AALSVLearnDemoCharacter::SetMovementState(EALS_MovementState NewMovementState)
{
	if (MovementState != NewMovementState)
	{
		// TODO MovementState改变之后对一些状态的影响

		MovementState = NewMovementState;
	}
}

void AALSVLearnDemoCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AALSVLearnDemoCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AALSVLearnDemoCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value * LookUpRate);
}

void AALSVLearnDemoCharacter::LookRight(float Value)
{
	AddControllerYawInput(Value * LookRightRate);
}

void AALSVLearnDemoCharacter::MoveWalk()
{
	switch (DesiredGait)
	{
	case EALS_Gait::EAG_Walking:
		DesiredGait = EALS_Gait::EAG_Running;
		break;
	case EALS_Gait::EAG_Running:
		DesiredGait = EALS_Gait::EAG_Walking;
		break;
	default: break;
	}
}

void AALSVLearnDemoCharacter::MoveSprint_Pressed()
{
	DesiredGait = EALS_Gait::EAG_Sprinting;
}

void AALSVLearnDemoCharacter::MoveSprint_Released()
{
	DesiredGait = EALS_Gait::EAG_Running;
}

void AALSVLearnDemoCharacter::ResetStanceActionCount()
{
	//////////////////////////////////////////////////////////////////// 第八次更新

	StanceActionCount = 0;

	//////////////////////////////////////////////////////////////////// 
}

void AALSVLearnDemoCharacter::StanceAction()
{
	// TODO Movement Action

	//////////////////////////////////////////////////////////////////// 第八次更新

	/*
	 * 敲击的第一次会触发切换动作的逻辑，在规定时间内后面敲击 >= 1次就会触发滚动效果。 
	 */
	if (MainAnimInstance->IsAnyMontagePlaying())
	{
		return;
	}
	StanceActionCount++;
	GetWorldTimerManager().SetTimer(TH_StanceAction, this, &AALSVLearnDemoCharacter::ResetStanceActionCount, .3f,
	                                false);

	if (StanceActionCount == 1)
	{
		//////////////////////////////////////////////////////////// 第七次更新

		switch (Stance)
		{
		case EALS_Stance::EAS_Standing:
			DesiredStance = EALS_Stance::EAS_Crouching;
			Crouch();
			break;
		case EALS_Stance::EAS_Crouching:
			DesiredStance = EALS_Stance::EAS_Standing;
			UnCrouch();
			break;
		}
		////////////////////////////////////////////////////////////
	}
	else if (IsValid(MainAnimInstance))
	{
		
		/*
		 * 播放动画蒙太奇，这里要将期待的Stance状态和原有的值相反，
		 * 因为只要到了这个条件中，那么一定触发了切换状态的逻辑，就会导致原本角色在站立状态滚动完之后变成了蹲伏状态，这并不是我们想要的结果。
		 * 所以就让状态和原有值相反，进行相抵消。
		 */
		
		MainAnimInstance->Montage_Play(GetRollAnimation(), 1.15);
		
		switch (Stance)
		{
		case EALS_Stance::EAS_Standing:
			DesiredStance = EALS_Stance::EAS_Crouching;
			break;
		case EALS_Stance::EAS_Crouching:
			DesiredStance = EALS_Stance::EAS_Standing;
			break;
		}
	}

	//////////////////////////////////////////////////////////////////// 
}

float AALSVLearnDemoCharacter::GetAnimCurveValue(FName CurveName)
{
	if (IsValid(MainAnimInstance)) return MainAnimInstance->GetCurveValue(CurveName);
	return 0.f;
}
