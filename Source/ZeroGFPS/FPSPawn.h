// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "FPSPawn.generated.h"

/**
 * 
 */
UCLASS()
class ZEROGFPS_API AFPSPawn : public ADefaultPawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump Parameters")
		float TimeJumpPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump Parameters")
		float MaxJumpForce = 10000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump Parameters")
		float MaxJumpTime = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Jump Parameters")
		UWorld* Wrld;

	UPROPERTY()
		FVector TargetDirection;

public:
	// Sets default values for this pawn's properties
	AFPSPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* SkeletalMesh;

protected:

	void SetupCapsule();
	void SetupCamera();
	void SetupMesh();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Val);
	void MoveRight(float Val);

	void JumpPressed();
	void JumpReleased();

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

	UFUNCTION()
		void Interact();

	UFUNCTION()
		void InteractReleased();
};
