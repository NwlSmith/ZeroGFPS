// Fill out your copyright notice in the Description page of Project Settings.


#include "TPPawnPlayerCameraManager.h"

void ATPPawnPlayerCameraManager::LimitViewPitch(FRotator& ViewRotation, float InViewPitchMin, float InViewPitchMax)
{
	//ViewRotation.Pitch = FMath::ClampAngle(ViewRotation.Pitch, InViewPitchMin, InViewPitchMax);
	ViewRotation.Pitch = FRotator::ClampAxis(ViewRotation.Pitch);
}

void ATPPawnPlayerCameraManager::LimitViewRoll(FRotator&  ViewRotation, float InViewRollMin, float InViewRollMax)
{
	//GetOwner()
	//ViewRotation.Roll = FMath::ClampAngle(ViewRotation.Roll, InViewRollMin, InViewRollMax);
	//ViewRotation.Roll = FRotator::ClampAxis(ViewRotation.Roll);
}

void ATPPawnPlayerCameraManager::LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax)
{
	//ViewRotation.Yaw = FMath::ClampAngle(ViewRotation.Yaw, InViewYawMin, InViewYawMax);
	//ViewRotation.Yaw = FRotator::ClampAxis(ViewRotation.Yaw);
}
