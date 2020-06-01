// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "TPPawnPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ZEROGFPS_API ATPPawnPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	virtual void LimitViewPitch(FRotator& ViewRotation, float InViewPitchMin, float InViewPitchMax) override;

	virtual void LimitViewRoll(FRotator& ViewRotation, float InViewRollMin, float InViewRollMax) override;

	virtual void LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax) override;
};
