// Fill out your copyright notice in the Description page of Project Settings.


#include "ALS_CameraManager.h"

#include "ALS_PlayerCameraBehavior.h"
// #include "ALSVLearnDemo/Interfaces/ALS_Camera.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AALS_CameraManager::AALS_CameraManager()
{
	CameraBehaviorComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CameraBehaviorComp"));
	CameraBehaviorComp->SetupAttachment(RootComponent);
}

/**
 * @brief 摄像机创建了之后，控制器会给他一个所控制的对象，如果这个对象具有 ”ALS_Character“ 标签的话，使用自己定义的摄像机移动逻辑。没有就用父类的。
 */
void AALS_CameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	if (!IsValid(OutVT.Target)) return;

	if (OutVT.Target->ActorHasTag(TEXT("ALS_Character")))
	{
		CustomCameraBehavior(OutVT.POV.Location, OutVT.POV.Rotation, OutVT.POV.FOV);
	}
	else
	{
		Super::UpdateViewTargetInternal(OutVT, DeltaTime);
	}
}

void AALS_CameraManager::OnPossess(APawn* NewPawn)
{
	ControllerPawn = NewPawn;
	CameraInterface = Cast<IALS_Camera>(ControllerPawn);
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/// 第二篇更新： 引用动画实例，将玩家控制器、控制的Pawn放入蓝图设置。

	PlayerCameraBehavior = Cast<UALS_PlayerCameraBehavior>(CameraBehaviorComp->GetAnimInstance());

	//////////////////////////////////////////////////////////////////////////////////////////////////

	if (PlayerCameraBehavior)
	{
		PlayerCameraBehavior->SetPlayerController(GetOwningPlayerController());
		PlayerCameraBehavior->SetControlledPawn(ControllerPawn);
	}
}

void AALS_CameraManager::CustomCameraBehavior(FVector& Location, FRotator& Rotation, float& FOV)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/// 第二篇更新：  自定义摄像机移动方式

	if (!CameraInterface) return;

	// 使用接口函数，获取轴心位置、 第一、三人称镜头所覆盖的范围
	// FOV是指镜头所能覆盖的范围，
	const FTransform PivotTarget = CameraInterface->Execute_Get3PPivotTarget(ControllerPawn);
	float TP_FOV, FP_FOV;
	CameraInterface->Execute_GetCameraParameters(ControllerPawn, TP_FOV, FP_FOV);

	// TODO 后面使用到了 RotationLagSpeed 一定要对文章进行更新
	// 更新摄像机目标旋转度的值
	const FRotator CurrentRotation = GetCameraRotation();
	const FRotator TargetRotation = GetOwningPlayerController()->GetControlRotation();
	TargetCameraRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->DeltaTimeSeconds,
	                                        GetCameraBehaviorParam(TEXT("RotationLagSpeed")));

	// 添加了 PivotLagSpeed 值之后，摄像机左右移动时会有一个平滑过渡的效果，向左向右移动，人物停止移动之后镜头得过一会才会停止。
	// 更新平滑目标平滑锚点位置信息 
	const FVector LagLocation = CalculateAxisIndependentLag(SmoothedPivotTarget.GetTranslation(),
	                                                        PivotTarget.GetTranslation(),
	                                                        TargetCameraRotation,
	                                                        FVector(GetCameraBehaviorParam(TEXT("PivotLagSpeed_X")),
	                                                                GetCameraBehaviorParam(TEXT("PivotLagSpeed_Y")),
	                                                                GetCameraBehaviorParam(TEXT("PivotLagSpeed_Z"))));
	SmoothedPivotTarget = FTransform(PivotTarget.GetRotation(), LagLocation, FVector(1.f, 1.f, 1.f));

	// 设置局部轴心偏移
	FVector ForwardVector = SmoothedPivotTarget.GetRotation().GetForwardVector() * GetCameraBehaviorParam(
		TEXT("PivotOffset_X"));
	FVector RightVector = SmoothedPivotTarget.GetRotation().GetRightVector() * GetCameraBehaviorParam(
		TEXT("PivotOffset_Y"));
	FVector UpVector = SmoothedPivotTarget.GetRotation().GetUpVector() * GetCameraBehaviorParam(TEXT("PivotOffset_Z"));
	PivotLocation = SmoothedPivotTarget.GetTranslation() + ForwardVector + RightVector + UpVector;

	// CameraOffset 存储的是摄像机与对象之间的位置偏差值。 就相当于之前用的角色后面跟随的摄像头的相对位置。
	// 设置局部摄像机偏移位置
	ForwardVector = TargetCameraRotation.Vector() * GetCameraBehaviorParam(TEXT("CameraOffset_X"));
	RightVector = UKismetMathLibrary::GetRightVector(TargetCameraRotation) * GetCameraBehaviorParam(
		TEXT("CameraOffset_Y"));
	UpVector = UKismetMathLibrary::GetUpVector(TargetCameraRotation) * GetCameraBehaviorParam(TEXT("CameraOffset_Z"));
	TargetCameraLocation = PivotLocation + ForwardVector + RightVector + UpVector;

	// step 6
	FVector StartLocation;
	float Radius;
	TEnumAsByte<ETraceTypeQuery> TraceChannel;
	CameraInterface->Execute_Get3PTraceParams(ControllerPawn, StartLocation, Radius, TraceChannel);

	// 发射一个从角色射向摄像机的射线， 如果碰撞到了物体并且射线没有将物体完全穿透，那么就要将镜头往角色位置移动一点
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLocation, TargetCameraLocation, Radius, TraceChannel,
	                                        false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);
	if (OutHit.bBlockingHit && !OutHit.bStartPenetrating)
	{
		TargetCameraLocation += OutHit.Location - OutHit.TraceEnd;
	}

	Location = TargetCameraLocation;
	Rotation = TargetCameraRotation;
	FOV = TP_FOV;

	//////////////////////////////////////////////////////////////////////////////////////////////////
}

float AALS_CameraManager::GetCameraBehaviorParam(FName CurveName)
{
	if (PlayerCameraBehavior)
	{
		return PlayerCameraBehavior->GetCurveValue(CurveName);
	}
	return 0.f;
}

FVector AALS_CameraManager::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation,
                                                        FRotator CameraRotation, FVector LagSpeeds)
{
	// 获取摄像头的横向旋转角度
	const FRotator CameraRotationYaw(0.f, CameraRotation.Yaw, 0.f);

	// RotatorVector : 让 Vector 向量顺时针绕着原点旋转Rotator个值
	// UnrotatorVector : 让 Vector 向量逆时针绕着原点旋转Rotator个值
	const FVector CurrentLocation_UnRotateVector = CameraRotationYaw.UnrotateVector(CurrentLocation);
	const FVector TargetLocation_UnRotateVector = CameraRotationYaw.UnrotateVector(TargetLocation);

	FVector Location;
	Location.X = FMath::FInterpTo(CurrentLocation_UnRotateVector.X, TargetLocation_UnRotateVector.X,
	                              GetWorld()->DeltaTimeSeconds, LagSpeeds.X);
	Location.Y = FMath::FInterpTo(CurrentLocation_UnRotateVector.Y, TargetLocation_UnRotateVector.Y,
	                              GetWorld()->DeltaTimeSeconds, LagSpeeds.Y);
	Location.Z = FMath::FInterpTo(CurrentLocation_UnRotateVector.Z, TargetLocation_UnRotateVector.Z,
	                              GetWorld()->DeltaTimeSeconds, LagSpeeds.Z);

	return CameraRotationYaw.RotateVector(Location);
}
