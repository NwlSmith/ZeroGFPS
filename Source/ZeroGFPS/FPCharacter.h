// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Math/UnrealMathUtility.h"
#include "FPCharacter.generated.h"


UCLASS()
class ZEROGFPS_API AFPCharacter : public ACharacter
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


public:
	// Sets default values for this character's properties
	AFPCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;

protected:
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
