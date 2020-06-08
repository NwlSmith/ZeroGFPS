// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "OrientationStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS(abstract)
class ZEROGFPS_API UOrientationStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target Pawn")
		APawn* TargetPawn;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Target")
		APawn* GetTargetPawn();

	UFUNCTION(BlueprintCallable, Category = "Target")
		void SetTargetPawn(APawn* NewTarget);

	//UFUNCTION(BlueprintCallable)
	virtual void CalculateOrientation() PURE_VIRTUAL(UOrientationStaticMeshComponent::CalculateOrientation, ;);
};
