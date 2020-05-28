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
		float RotationSpeed = 75.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float InAirMovementDampener = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float SurfaceTransitionThreshold = 0.985f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float MoveMouseTransitionBeginTime = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		float TimeAtLastMoveMouse = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		bool OnGround = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		FVector PreviousLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		FVector RealSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float MaxJumpForce = 100000.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float MaxJumpTime = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float GravityScale = 9800.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float CurGravityScale = 9800.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump Parameters")
		float TimeJumpPressed = 0.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupCapsule();
	void SetupCamera();
	void SetupMesh();
	void SetupMovementComponent();

	void UpdateRealSpeed();
	void InAirSurfaceCalculations();
	void OnGroundSurfaceCalculations();
	bool CheckIfCanTransitionToNewSurface(FHitResult Hit);
	bool CheckIfNeedToTransitionToNewSurface(FVector HitNormal);
	void TransitionToNewSurface(FHitResult Hit);
	void GroundTransitionCalculations(bool TraceHitObj);

	void NotifyHit
	(
		UPrimitiveComponent * MyComp,
		AActor * Other,
		UPrimitiveComponent * OtherComp,
		bool bSelfMoved,
		FVector HitLocation,
		FVector HitNormal,
		FVector NormalImpulse,
		const FHitResult & Hit
	);

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	virtual void AddControllerPitchInput(float AxisValue) override;
	virtual void AddControllerYawInput(float AxisValue) override;

	void JumpPressed();
	void JumpReleased();

	void Interact();
	void InteractReleased();

	void StdPrint(FString message);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;


	UFUNCTION()
		float CalculateHorizontalMovementValue();

};
