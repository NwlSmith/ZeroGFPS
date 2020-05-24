// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FPMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "FPPawn.generated.h"

UCLASS()
class ZEROGFPS_API AFPPawn : public APawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Jump Parameters")
		UWorld* Wrld;

	UPROPERTY()
		FVector TargetDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capsule")
		UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MovementComponent")
	class UFPMovementComponent* MovementComponent;




public:
	// Sets default values for this pawn's properties
	AFPPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float TimeJumpPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float MaxJumpForce = 10000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Parameters")
		float MaxJumpPressTime = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump Parameters")
		bool Jumping = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float MoveSpeed = 4000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float MaxSpeed = 600.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float InAirMovementDampener = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float MovementFriction = 0.175f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement Parameters")
		float maxSlopeAngle = 35.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		FVector PrevNormal = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Parameters")
		bool OnGround = true;

protected:

	void SetupCapsule();
	void SetupCamera();
	void SetupMesh();
	void SetupMovementComponent();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void JumpPressed();
	void JumpReleased();

	bool CheckIsOnGround();

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	UFUNCTION()
		void Interact();

	UFUNCTION()
		void InteractReleased();


	UFUNCTION()
		float CalculateHorizontalMovementValue();
};
