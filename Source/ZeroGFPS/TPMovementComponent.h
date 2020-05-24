// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPPawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TPMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ZEROGFPS_API UTPMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};