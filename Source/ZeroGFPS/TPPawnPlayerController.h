// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPPawnPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ZEROGFPS_API ATPPawnPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATPPawnPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void AddPitchInput(float Val) override;

	virtual void AddYawInput(float Val) override;
};
