// Fill out your copyright notice in the Description page of Project Settings.


#include "TPPawnPlayerController.h"
/*
	Override PlayerControl class to re-orient the camera properly
*/

void ATPPawnPlayerController::AddPitchInput(float Val)
{
	//FVector ActorRight = GetOwner()->GetActorRightVector();
	//FRotator NewRot = FRotator::FRotator(Val * ActorRight.X, Val * ActorRight.Y, Val * ActorRight.Z);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("NewRot : %s"), *NewRot.ToString()));
	//RotationInput += NewRot;
	/*FRotator NewRot = FRotator::FRotator(0.f, Val, 0.f);

	NewRot.RotateVector(GetOwner()->GetActorRightVector().Rotation());
	RotationInput += NewRot;*/
	// Change to move pitch AND Yaw AND roll based on up vector
	RotationInput.Pitch += !IsLookInputIgnored() ? Val * InputPitchScale : 0.f;
}

void ATPPawnPlayerController::AddYawInput(float Val)
{
	RotationInput.Yaw += !IsLookInputIgnored() ? Val * InputYawScale : 0.f;
}
