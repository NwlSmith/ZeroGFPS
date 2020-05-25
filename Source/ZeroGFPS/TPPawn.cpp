// Fill out your copyright notice in the Description page of Project Settings.


#include "TPPawn.h"

// Sets default values
ATPPawn::ATPPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SetupCapsule();
	SetupCamera();
	SetupMesh();
	SetupMovementComponent();

	// Take control of the default player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

void ATPPawn::SetupCapsule()
{
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("Capsule")));
	Capsule->InitCapsuleSize(40.0f, 92.0f);
	Capsule->SetSimulatePhysics(true);
	Capsule->SetEnableGravity(false);
	Capsule->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	RootComponent = Capsule;
}

void ATPPawn::SetupCamera()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName(TEXT("SpringArm")));
	SpringArm->SetupAttachment(RootComponent);


	Camera = CreateDefaultSubobject<UCameraComponent>(FName(TEXT("Camera")));
	Camera->SetupAttachment(SpringArm);
}

void ATPPawn::SetupMesh()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("SkeletalMesh")));
	SkeletalMesh->SetOwnerNoSee(false);
	SkeletalMesh->SetupAttachment(RootComponent);
}

void ATPPawn::SetupMovementComponent()
{
	// Create an instance of our movement component, and tell it to update the root.
	MovementComponent = CreateDefaultSubobject<UTPMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void ATPPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATPPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (SkeletalMesh)
	{
		const FVector Start = SkeletalMesh->GetSocketLocation("center");

		const FVector FLSocketLocation = SkeletalMesh->GetSocketLocation("frontLeft");
		const FVector FRSocketLocation = SkeletalMesh->GetSocketLocation("frontRight");

		FHitResult HitDataFL(ForceInit);
		FHitResult HitDataFR(ForceInit);

		FCollisionQueryParams CollisionParams;

		//GetCharacterMovement()->GravityScale = 0.0f;
		
		if (GetWorld()->LineTraceSingleByChannel(HitDataFL, Start, FLSocketLocation, ECC_Visibility, CollisionParams))
		{
			//Print out the name of the traced actor
			if (HitDataFL.GetActor())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Front LEFT hit name: %s"), *HitDataFL.GetActor()->GetName()));
			}
		}

		if (GetWorld()->LineTraceSingleByChannel(HitDataFR, Start, FRSocketLocation, ECC_Visibility, CollisionParams))
		{
			//Print out the name of the traced actor
			if (HitDataFR.GetActor())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Front RIGHT hit name: %s"), *HitDataFR.GetActor()->GetName()));
			}
		}

		FHitResult HitDataD(ForceInit);
		// use this to make player stick to wall
		if (GetWorld()->LineTraceSingleByChannel(HitDataD, Start, Start - GetActorUpVector() * (Capsule->GetScaledCapsuleHalfHeight() + 10.0f), ECC_Visibility, CollisionParams))
		{
			//Print out the name of the traced actor
			if (HitDataD.GetActor())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("DOWN hit name: %s"), *HitDataD.GetActor()->GetName()));
			}
		}


		FHitResult HitDataF(ForceInit);
		// use this to make player stick to wall
		if (GetWorld()->LineTraceSingleByChannel(HitDataF, Start, SkeletalMesh->GetSocketLocation("forward") , ECC_Visibility, CollisionParams))
		{
			//Print out the name of the traced actor
			if (HitDataF.GetActor())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Forward hit name: %s"), *HitDataF.GetActor()->GetName()));
			}
		}



		if (CheckIfNeedToTransitionToNewSurface(HitDataD.Normal))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Transitioning to DOWN vector")));
			TransitionToNewSurface(HitDataD);
		}
		else if (CheckIfNeedToTransitionToNewSurface(HitDataF.Normal))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Transitioning to FORWARD vector")));
			//TransitionToNewSurface(HitDataF);
		}

		
		if (CheckIfNeedToTransitionToNewSurface(HitDataFL.Normal))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Transitioning to left vector")));
			//TransitionToNewSurface(HitDataFL);
		}
		else if (CheckIfNeedToTransitionToNewSurface(HitDataFL.Normal))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Transitioning to right vector")));
			//TransitionToNewSurface(HitDataFL);
		}
			
		Capsule->AddForce(-GetActorUpVector() * 9800);
	}
}

// Called to bind functionality to input
void ATPPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	// Horizontal movement
	PlayerInputComponent->BindAxis("Forward", this, &ATPPawn::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &ATPPawn::MoveRight);

	// Mouse Axis Binding
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	// Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPPawn::JumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATPPawn::JumpReleased);

	// Simple Action
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ATPPawn::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ATPPawn::InteractReleased);
}

bool ATPPawn::CheckIfNeedToTransitionToNewSurface(FVector HitNormal)
{
	return HitNormal.Y <= GetActorUpVector().Y - SurfaceTransitionThreshold ||
		HitNormal.Y >= GetActorUpVector().Y + SurfaceTransitionThreshold ||
		HitNormal.X <= GetActorUpVector().X - SurfaceTransitionThreshold ||
		HitNormal.X >= GetActorUpVector().X + SurfaceTransitionThreshold;
}

void ATPPawn::TransitionToNewSurface(FHitResult HitNormal)
{
	Capsule->SetPhysicsAngularVelocity(FVector::ZeroVector);

	//Grab a ref to the world here
	UWorld* world = GetWorld();

	//Trace to get the surface normal in front of actor
	FVector newUp = HitNormal.ImpactNormal;
	FVector point = HitNormal.ImpactPoint + newUp * (Capsule->GetScaledCapsuleHalfHeight() + 1.0f);

	//Some math to get the new Axis
	FVector curRight = GetActorRightVector();
	FVector newForward = FVector::CrossProduct(curRight, newUp);
	FVector newRight = FVector::CrossProduct(newUp, newForward);

	//Build the new transform!
	FTransform newTransform(newForward, newRight, newUp, point);
	SetActorTransform(newTransform);
}

void ATPPawn::MoveForward(float AxisValue)
{
	if (AxisValue != 0)
	{
		//Capsule->AddForce(GetActorForwardVector() * MoveSpeed * 10.0f * Val);
		//AddMovementInput(GetActorForwardVector(), AxisValue, true);
		if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
		{
			float movementValue = CalculateHorizontalMovementValue();
			MovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);

			if (GetWorld()->GetTimeSeconds() - TimeAtLastMoveMouse >= MoveMouseTransitionBeginTime)
			{

				FRotator NewRot = FMath::QInterpTo(GetControlRotation().Quaternion(), GetActorRotation().Quaternion(), 0.25f, .1f).Rotator();
				GetController()->SetControlRotation(NewRot);
			}
		}
	}
}

void ATPPawn::MoveRight(float AxisValue)
{
	if (AxisValue != 0)
	{
		// This will be commented out for now. In the future, I want the directional keys to make the player turn to move in that direction.
		/*if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
		{
			MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
		}*/
		FTransform NewTransform = GetTransform();

		FRotator rot = FRotator(0, AxisValue * RotationSpeed / 30.0f, 0);

		NewTransform.ConcatenateRotation(rot.Quaternion());
		NewTransform.NormalizeRotation();
		SetActorTransform(NewTransform);
	}
}

float ATPPawn::CalculateHorizontalMovementValue()
{
	float MoveVal = MaxSpeed;

	if (!OnGround) { MoveVal *= InAirMovementDampener; }

	return MoveVal;
}

void ATPPawn::AddControllerPitchInput(float AxisValue)
{
	Super::AddControllerPitchInput(AxisValue);
	if (AxisValue != 0)
	{
		TimeAtLastMoveMouse = GetWorld()->GetTimeSeconds();
	}
}

void ATPPawn::AddControllerYawInput(float AxisValue)
{
	Super::AddControllerYawInput(AxisValue);
	if (AxisValue != 0)
	{
		TimeAtLastMoveMouse = GetWorld()->GetTimeSeconds();
	}
}

void ATPPawn::JumpPressed()
{
	if (OnGround)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Jump Pressed")));
		//TimeJumpPressed = Wrld->GetTimeSeconds();
		// Trigger lowering and shaking animation
	}

}

void ATPPawn::JumpReleased()
{
	if (OnGround)
	{
		float jumpPower = 1000.0f;
		//jumpPower = MaxJumpForce * FMath::Clamp(Wrld->GetTimeSeconds() - TimeJumpPressed, 0.0f, MaxJumpTime) / MaxJumpTime;

		//LaunchCharacter(GetActorUpVector() * jumpPower, false, false);

		//GetCharacterMovement()->GravityScale = 0.0f;
		// Stop lowering and shaking animation.
	}
}

void ATPPawn::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("E was pressed"));
	Capsule->AddForce(GetActorUpVector() * -1.0f * 9800000.0f);
}

void ATPPawn::InteractReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("E was released"));
}

