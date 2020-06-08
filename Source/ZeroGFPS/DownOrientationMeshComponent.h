// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrientationStaticMeshComponent.h"
#include "DownOrientationMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class ZEROGFPS_API UDownOrientationMeshComponent : public UOrientationStaticMeshComponent
{
	GENERATED_BODY()

public:

	void CalculateOrientation();
};
