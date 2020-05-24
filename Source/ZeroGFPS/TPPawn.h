// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TPMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "TPPawn.generated.h"

UCLASS()
class ZEROGFPS_API ATPPawn : public APawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capsule")
		UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringArm")
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementComponent")
		class UTPMovementComponent* MovementComponent;

public:
	// Sets default values for this pawn's properties
	ATPPawn();

	bool Jumping = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float MoveSpeed = 4000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float MaxSpeed = 600.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float InAirMovementDampener = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		bool OnGround = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupCapsule();
	void SetupCamera();
	void SetupMesh();
	void SetupMovementComponent();

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void JumpPressed();
	void JumpReleased();

	void Interact();
	void InteractReleased();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION()
		float CalculateHorizontalMovementValue();

};
