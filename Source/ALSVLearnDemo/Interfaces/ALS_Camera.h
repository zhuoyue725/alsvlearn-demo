// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALS_Camera.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UALS_Camera : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ALSVLEARNDEMO_API IALS_Camera
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**
	 * @brief 返回Actor的Transform变量
	 * 包含所在位置、旋转、缩放等信息。
	 */
	UFUNCTION(BlueprintNativeEvent)
	FTransform Get3PPivotTarget();

	virtual FTransform Get3PPivotTarget_Implementation();

	/**
	 * @brief 获得摄像机的一些视场角属性，视场角越大视野就越大。
	 * @param TP_FOV 第三人称视场角
	 * @param FP_FOV 第一人称市场角
	 * @return 是否是右边位置。
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool GetCameraParameters(float& TP_FOV, float& FP_FOV);

	virtual bool GetCameraParameters_Implementation(float& TP_FOV, float& FP_FOV);
	
	////////////////////////////////////////////////// 第五次更新
	
	UFUNCTION(BlueprintNativeEvent)
	void Get3PTraceParams(FVector& TraceOrigin, float& TraceRadius, TEnumAsByte<ETraceTypeQuery>& TraceChanel);

	virtual void Get3PTraceParams_Implementation(FVector& TraceOrigin, float& TraceRadius, TEnumAsByte<ETraceTypeQuery>& TraceChanel); 
	
	////////////////////////////////////////////////// 

};