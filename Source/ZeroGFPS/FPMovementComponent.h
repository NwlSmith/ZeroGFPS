// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPPawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "FPMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ZEROGFPS_API UFPMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};
