// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/UnrealMathUtility.h"
#include "TPPawn.h"
#include "MeshCapture.generated.h"

UCLASS()
class ZEROGFPS_API AMeshCapture : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root")
		USceneComponent* ThisRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
		UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringArm")
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SceneCaptureComponent2D")
		USceneCaptureComponent2D* Capture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SceneCaptureComponent2D")
		ATPPawn* PlayerPawn;

public:	
	// Sets default values for this actor's properties
	AMeshCapture();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupRoot();
	void SetupMesh();
	void SetupCapture();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
