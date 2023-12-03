// Fill out your copyright notice in the Description page of Project Settings.


#include "ALS_Camera.h"

// Add default functionality here for any IALS_Camera functions that are not pure virtual.
FTransform IALS_Camera::Get3PPivotTarget_Implementation()
{
	return {};
}

bool IALS_Camera::GetCameraParameters_Implementation(float& TP_FOV, float& FP_FOV)
{
	return false;
}

void IALS_Camera::Get3PTraceParams_Implementation(FVector& TraceOrigin, float& TraceRadius,
                                                  TEnumAsByte<ETraceTypeQuery>& TraceChanel)
{
}
