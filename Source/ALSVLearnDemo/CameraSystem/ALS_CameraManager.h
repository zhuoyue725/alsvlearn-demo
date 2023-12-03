// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ALSVLearnDemo/Interfaces/ALS_Camera.h"
#include "Camera/PlayerCameraManager.h"
#include "ALS_CameraManager.generated.h"

class UALS_PlayerCameraBehavior;
/**
 * 
 */
UCLASS()
class ALSVLEARNDEMO_API AALS_CameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AALS_CameraManager();
	
	UFUNCTION(BlueprintCallable)
	void OnPossess(APawn* NewPawn);

	// 更新摄像机位置的函数
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;
	
	/**
	 * @brief 自定义摄像机的信息
	 * @param Location 摄像机的位置
	 * @param Rotation 摄像机的旋转值
	 * @param FOV 摄像机镜头所覆盖的位置
	 */
	UFUNCTION(BlueprintCallable)
	void CustomCameraBehavior(FVector& Location, FRotator& Rotation, float& FOV);
	
	/**
	 * @brief 获取摄像机曲线的值
	 */
	UFUNCTION(BlueprintCallable)
	float GetCameraBehaviorParam(FName CurveName);

	/**
	 * @brief 计算轴无关延迟
	 * @param CurrentLocation 目前位置
	 * @param TargetLocation  目标位置
	 * @param CameraRotation  摄像机位置
	 * @param LagSpeeds  延迟速度
	 * @return 延迟之后的位置
	 */
	UFUNCTION(BlueprintCallable)
	FVector CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeeds);
	
	
	/**
	 * @brief 摄像机将要旋转到的旋转度  也就相当于当前摄像机的旋转值
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator TargetCameraRotation;

	
	/**
	 * @brief 摄像机将要移动到的位置 相当于当前摄像机的位置
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector TargetCameraLocation;

	/**
	 * @brief 平滑目标锚点信息
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform SmoothedPivotTarget;

	/**
	 * @brief 设置锚点位置信息 TODO 与上面变量有什么区别？
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector PivotLocation;
	
	////////////////////////////////////////////////////////////////////////////////////////
	/// 组件相关属性
	
	UFUNCTION(Category="Components", BlueprintCallable)
	USkeletalMeshComponent* GetCameraBehaviorComp() const { return CameraBehaviorComp; }
	
	///////////////////////////////////////////////////////////////////////////////////////
	/// 对一些类的引用

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APawn* ControllerPawn;

	UPROPERTY()
	UALS_PlayerCameraBehavior* PlayerCameraBehavior;

	IALS_Camera* CameraInterface;
	
private:
	/**
	 * @brief 添加摄像机骨骼，这样的话，人物运行状态机的时候摄像机也会进行状态机，这样就会有一个相对高的匹配度。
	 */
	UPROPERTY(Category="Components", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* CameraBehaviorComp;
};
