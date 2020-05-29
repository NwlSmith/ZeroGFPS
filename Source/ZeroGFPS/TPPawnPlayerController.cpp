// Fill out your copyright notice in the Description page of Project Settings.


#include "TPPawnPlayerController.h"
/*
	Override PlayerControl class to re-orient the camera properly
*/

ATPPawnPlayerController::ATPPawnPlayerController(const FObjectInitializer& ObjectInitializer)
{
	//Super::APlayerController(ObjectInitializer);

	//PlayerCameraManagerClass = 
}

void ATPPawnPlayerController::AddPitchInput(float Val)
{
	RotationInput.Pitch += !IsLookInputIgnored() ? Val * InputPitchScale : 0.f;
}

void ATPPawnPlayerController::AddYawInput(float Val)
{
	RotationInput.Yaw += !IsLookInputIgnored() ? Val * InputYawScale : 0.f;
}