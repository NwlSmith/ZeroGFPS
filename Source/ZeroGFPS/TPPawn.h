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
#include "TPPawnPlayerController.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float MaxSpeed = 600.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float InAirMovementDampener = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float SurfaceTransitionThreshold = 0.99f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float LastWallTransitionTimeInterval = 0.01f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float TimeAtLastWallTransition = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float MoveMouseTransitionBeginTime = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		float TimeAtLastMoveMouse = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		bool bOnGround = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		bool bSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		float SprintingMoveMult = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		FVector PreviousLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		FVector RealSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		FRotator RelRot = FRotator(0.f, -90.f, 0.f);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		FVector RelPos = FVector(0.f, 0.f, -90.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket Locations")
		FVector SocketCenter = FVector(0.f, 0.f, 0.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket Locations")
		FVector SocketForward = FVector(50.f, 0.f, -101.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket Locations")
		FVector SocketRight = FVector(0, 50.f, -101.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket Locations")
		FVector SocketBack = FVector(-50.f, 0.f, -101.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket Locations")
		FVector SocketLeft = FVector(0.f, -50.f, -101.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Socket Locations")
		FVector SocketDown = FVector(0.f, 0.f, -121.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float MaxJumpForce = 100000.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float MaxJumpTime = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump Parameters")
		float TimeJumpPressed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float GravityScale = 9800.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float CurGravityScale = 9800.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump Parameters")
		bool bJumping = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health Info")
		float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health Info")
		float CurHealth = 100.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupCapsule();
	void SetupCamera();
	void SetupMesh();
	void SetupMovementComponent();

	void MeshLerp();
	void UpdateRealSpeed();
	void InAirSurfaceCalculations();
	void OnGroundSurfaceCalculations();
	bool CheckIfCanTransitionToNewSurface(const FHitResult Hit) const;
	bool CheckIfNeedToTransitionToNewSurface(const FVector HitNormal) const;
	void CalculateSurfaceTransition(const FVector Socket);
	void TransitionToNewSurface(const FHitResult Hit);
	void GroundTransitionCalculations(const bool TraceHitObj);
	FVector RelativeSocketLocation(const FVector Socket);

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

	void MoveForward(const float AxisValue);
	void MoveRight(const float AxisValue);

	virtual void AddControllerPitchInput(float AxisValue) override;
	virtual void AddControllerYawInput(float AxisValue) override;

	void JumpPressed();
	void JumpReleased();

	void SprintPressed();
	void SprintReleased();

	void Interact();
	void InteractReleased();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;


	UFUNCTION()
		float CalculateHorizontalMovementValue();

	UFUNCTION(BlueprintCallable)
		float CalculateHealthFraction();

	UFUNCTION(BlueprintCallable)
		float CalculateJumpHoldFraction();

	UFUNCTION(BlueprintCallable)
		void UpdateHealth(float Delta);

	UFUNCTION(BlueprintCallable)
		void KillPawn();

	UFUNCTION()
		void StdPrint(const FString message);


};
