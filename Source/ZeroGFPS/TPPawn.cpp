// Fill out your copyright notice in the Description page of Project Settings.


#include "TPPawn.h"
#include "DrawDebugHelpers.h"

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

	PreviousLocation = GetActorLocation();

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
	SkeletalMesh->SetRelativeLocationAndRotation(RelPos, RelRot);
	//SkeletalMesh->SetupAttachment(RootComponent);
}

void ATPPawn::SetupMovementComponent()
{
	// Create an instance of our movement component, and tell it to update the root.
	MovementComponent = CreateDefaultSubobject<UTPMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
}

UPawnMovementComponent* ATPPawn::GetMovementComponent() const
{
	return MovementComponent;
}

// Called when the game starts or when spawned
void ATPPawn::BeginPlay()
{
	Super::BeginPlay();
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

	// Sprint Activation
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATPPawn::SprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATPPawn::SprintReleased);

	// Simple Action
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ATPPawn::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ATPPawn::InteractReleased);

}

// Called every frame
void ATPPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (SkeletalMesh)
	{
		//GetActorUpVector().ToOrientationQuat().Euler();
		//StdPrint(FString::Printf(TEXT("Up Rotation: %s"), *GetActorUpVector().ToString()));

		MeshLerp();
		
		UpdateRealSpeed();

		if (bOnGround)
			OnGroundSurfaceCalculations();
		else
			InAirSurfaceCalculations();
		Capsule->AddForce(-GetActorUpVector() * CurGravityScale);

		if (GetWorld()->GetTimeSeconds() - TimeAtLastMoveMouse >= MoveMouseTransitionBeginTime)
		{

			FRotator NewRot = FMath::QInterpTo(GetControlRotation().Quaternion(), GetActorRotation().Quaternion(), 0.25f, .1f).Rotator();
			GetController()->SetControlRotation(NewRot);
		}
	}
}

void ATPPawn::MeshLerp()
{
	//FTransform NewTrans = FTransform(Capsule->GetComponentTransform());

	//NewTrans.AddToTranslation(GetActorRotation().RotateVector(RelPos));
	//NewTrans.ConcatenateRotation(RelRot.Quaternion());

	FQuat TargetRot = Capsule->GetComponentRotation().Quaternion() * RelRot.Quaternion();
	FVector TargetPos = Capsule->GetComponentLocation() + GetActorRotation().RotateVector(RelPos);

	FQuat NewRot = FQuat::Slerp(SkeletalMesh->GetComponentRotation().Quaternion(), TargetRot, .2f);
	FVector NewPos = FMath::Lerp(SkeletalMesh->GetComponentLocation(), TargetPos, .2f);

	FTransform NewTrans = FTransform(NewRot, NewPos, FVector(1.f, 1.f, 1.f));

	SkeletalMesh->SetWorldTransform(NewTrans);
}

void ATPPawn::UpdateRealSpeed()
{
	RealSpeed = (GetActorLocation() - PreviousLocation) / GetWorld()->GetDeltaSeconds();
	PreviousLocation = GetActorLocation();
}

void ATPPawn::InAirSurfaceCalculations()
{
	// create tarray for hit results
	TArray<FHitResult> OutHits;

	// create a collision sphere
	FCollisionShape Sphere = FCollisionShape::MakeSphere(110.0f);

	// draw collision sphere
	//DrawDebugSphere(GetWorld(), GetActorLocation(), Sphere.GetSphereRadius(), 50, FColor::Purple, true);

	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_Visibility, Sphere);

	// Update OnGround
	GroundTransitionCalculations(isHit);

	if (isHit)
		TransitionToNewSurface(OutHits[0]);
}

void ATPPawn::OnGroundSurfaceCalculations()
{
	float forwardDot = FVector::DotProduct(RealSpeed, GetActorForwardVector());
	float rightDot = FVector::DotProduct(RealSpeed, GetActorRightVector());

	if (GetWorld()->TimeSeconds - TimeAtLastWallTransition >= LastWallTransitionTimeInterval)
	{
		// If pawn moving forward
		if (FMath::Abs(forwardDot) > FMath::Abs(rightDot) && forwardDot > 0)
		{
			CalculateSurfaceTransition(SocketForward);
		}
		else if (FMath::Abs(forwardDot) > FMath::Abs(rightDot) && forwardDot < 0)
		{
			CalculateSurfaceTransition(SocketBack);
		}
		else if (FMath::Abs(forwardDot) < FMath::Abs(rightDot) && rightDot > 0)
		{
			CalculateSurfaceTransition(SocketRight);
		}
		else if (FMath::Abs(forwardDot) < FMath::Abs(rightDot) && rightDot < 0)
		{
			CalculateSurfaceTransition(SocketLeft);
		}
	}
}

void ATPPawn::CalculateSurfaceTransition(const FVector Socket)
{
	//StdPrint(FString::Printf(TEXT("Moving : %s"), *socket));
	const FVector Center = RelativeSocketLocation(SocketCenter);
	const FVector Under = RelativeSocketLocation(SocketDown);
	const FVector TargetSoc = RelativeSocketLocation(Socket);

	FHitResult HitDataD(ForceInit);  // Check downwards
	FHitResult HitDataUp(ForceInit);  // Check wall transition
	FHitResult HitDataUnder(ForceInit);  // Check cliff transition

	FCollisionQueryParams CollisionParams;

	//StdPrint(FString::Printf(TEXT("Moving FORWARD")));

	// Perform line traces
	bool bHitGround = GetWorld()->LineTraceSingleByChannel(HitDataD, Center, Under, ECC_Visibility, CollisionParams);
	GetWorld()->LineTraceSingleByChannel(HitDataUp, Center, TargetSoc, ECC_Visibility, CollisionParams);
	GetWorld()->LineTraceSingleByChannel(HitDataUnder, TargetSoc, Under, ECC_Visibility, CollisionParams);

	GroundTransitionCalculations(bHitGround);

	// Firstly, keep the pawn upright.
	if (CheckIfCanTransitionToNewSurface(HitDataD))
	{
		StdPrint(FString::Printf(TEXT("%s hit name: %s"), *Socket.ToString(), *HitDataD.GetActor()->GetName()));
		StdPrint(FString::Printf(TEXT("Transitioning to DOWN vector")));
		TransitionToNewSurface(HitDataD);
	}
	// If there is a wall in front of the pawn, transition to it
	else if (CheckIfCanTransitionToNewSurface(HitDataUp))
	{
		StdPrint(FString::Printf(TEXT("%s up hit name: %s"), *Socket.ToString(), *HitDataUp.GetActor()->GetName()));
		StdPrint(FString::Printf(TEXT("Transitioning to %s up vector"), *Socket.ToString()));
		TransitionToNewSurface(HitDataUp);
	}
	// If there is nothing in front of the player, and the player is on a ledge, transition to it.
	else if (!HitDataUp.GetActor() && CheckIfCanTransitionToNewSurface(HitDataUnder))
	{
		StdPrint(FString::Printf(TEXT("%s -> under hit name: %s"), *Socket.ToString(), *HitDataUnder.GetActor()->GetName()));
		StdPrint(FString::Printf(TEXT("Transitioning to %s -> under vector"), *Socket.ToString()));
		TransitionToNewSurface(HitDataUnder);
	}
}

bool ATPPawn::CheckIfCanTransitionToNewSurface(const FHitResult Hit) const
{
	return Hit.GetActor() && CheckIfNeedToTransitionToNewSurface(Hit.Normal);
}

bool ATPPawn::CheckIfNeedToTransitionToNewSurface(const FVector HitNormal) const
{
	//StdPrint(FString::Printf(TEXT("Normal : %s Up : %s"), *HitNormal.ToString(), *GetActorUpVector().ToString()));
	//StdPrint(FString::Printf(TEXT("Dot : %f"), FVector::DotProduct(HitNormal, GetActorUpVector())));
	return FVector::DotProduct(HitNormal, GetActorUpVector()) < SurfaceTransitionThreshold;
}

void ATPPawn::TransitionToNewSurface(const FHitResult Hit)
{
	//Trace to get the surface normal in front of actor
	FVector newUp = Hit.Normal;
	FVector point = Hit.ImpactPoint + newUp * (Capsule->GetScaledCapsuleHalfHeight() + 1.0f);

	//Some math to get the new Axis
	FVector curRight = GetActorRightVector();
	FVector newForward = FVector::CrossProduct(curRight, newUp);
	FVector newRight = FVector::CrossProduct(newUp, newForward);

	//Build the new transform!
	FTransform newTransform(newForward, newRight, newUp, point);
	SetActorTransform(newTransform);
	SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));

	Capsule->SetPhysicsLinearVelocity(FVector::ZeroVector);
	Capsule->SetPhysicsAngularVelocity(FVector::ZeroVector);

	TimeAtLastWallTransition = GetWorld()->GetTimeSeconds();
}

void ATPPawn::GroundTransitionCalculations(const bool TraceHitObj)
{
	// If was on the ground last tick and is now not on the ground...
	if (bOnGround && !TraceHitObj)
	{
		// Turn off gravity
		CurGravityScale = 0.0f;
		StdPrint(FString::Printf(TEXT("WAS ON GROUND AND NOW IS NOT")));
		bSprinting = false;
	}
	// If was NOT on ground last tick and is now on ground...
	else if (!bOnGround && TraceHitObj)
	{
		// Turn on gravity and get rid of any velocity.
		CurGravityScale = GravityScale;
		Capsule->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Capsule->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		StdPrint(FString::Printf(TEXT("WAS NOT ON GROUND AND NOW IS")));
	}
	bOnGround = TraceHitObj;
}

FVector ATPPawn::RelativeSocketLocation(const FVector Socket)
{
	return Capsule->GetComponentLocation() + GetActorRotation().RotateVector(Socket);
}

void ATPPawn::NotifyHit
(
	UPrimitiveComponent * MyComp,
	AActor * Other,
	UPrimitiveComponent * OtherComp,
	bool bSelfMoved,
	FVector HitLocation,
	FVector HitNormal,
	FVector NormalImpulse,
	const FHitResult & Hit
)
{
	// Don't do in-air collision calculations when pawn is not in the air.
	if (bOnGround)
		return;

	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	//TransitionToNewSurface(HitNormal);


	UE_LOG(LogTemp, Warning, TEXT("Actor was hit"));
}

void ATPPawn::MoveForward(const float AxisValue)
{
	if (AxisValue != 0)
	{
		//Capsule->AddForce(GetActorForwardVector() * MoveSpeed * 10.0f * Val);
		//AddMovementInput(GetActorForwardVector(), AxisValue, true);
		if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
		{
			FVector movementDirection= FVector::VectorPlaneProject(GetControlRotation().Vector(), GetActorUpVector()).GetSafeNormal();


			const FVector Center = RelativeSocketLocation(SocketCenter);
			const FVector Under = RelativeSocketLocation(SocketDown);
			FHitResult HitDataD(ForceInit);  // Check downwards
			FCollisionQueryParams CollisionParams;
			if (GetWorld()->LineTraceSingleByChannel(HitDataD, Center, Under, ECC_Visibility, CollisionParams))
			{
				movementDirection = FVector::VectorPlaneProject(GetControlRotation().Vector(), HitDataD.ImpactNormal).GetSafeNormal();
			}

			

			// ALSO THIS IS ROTATING THE PAWN
			
			//FRotator NewRot = FMath::QInterpTo(GetActorRotation().Quaternion(), movementDirection.ToOrientationQuat(), 0.5f, .25f).Rotator();
			//FVector NewVec = GetActorForwardVector().RotateAngleAxis(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), movementDirection)), GetActorUpVector());
			//SetActorRotation(NewVec.Rotation());

			//Trace to get the surface normal in front of actor
			FVector newUp = GetActorUpVector();
			if (bOnGround)
				newUp = HitDataD.ImpactNormal;
			FVector point = GetActorLocation();// +newUp * (Capsule->GetScaledCapsuleHalfHeight() + 1.0f);

			//Some math to get the new Axis
			FVector newRight = FVector::CrossProduct(newUp, movementDirection);

			//Build the new transform!
			FTransform newTransform(movementDirection, newRight, newUp, point);
			SetActorTransform(newTransform);
			SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));

			MovementComponent->AddInputVector(movementDirection * AxisValue);
		}
	}
}

void ATPPawn::MoveRight(const float AxisValue)
{
	if (AxisValue != 0)
	{
		// This will be commented out for now. In the future, I want the directional keys to make the player turn to move in that direction.
		if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
		{
			MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
		}/*
		FTransform NewTransform = GetTransform();

		FRotator rot = FRotator(0, AxisValue * 75.0f / 30.0f, 0);

		NewTransform.ConcatenateRotation(rot.Quaternion());
		NewTransform.NormalizeRotation();
		SetActorTransform(NewTransform);*/
	}
}

float ATPPawn::CalculateHorizontalMovementValue()
{
	float MoveVal = MaxSpeed;

	if (!bOnGround)
	{
		MoveVal *= InAirMovementDampener;
	}
	else if (bSprinting)
	{
		MoveVal *= SprintingMoveMult;
	}

	return MoveVal;
}

void ATPPawn::AddControllerPitchInput(float AxisValue)
{
	//FVector ActorRight = GetActorRightVector() * AxisValue;
	//FRotator NewRot = FRotator::FRotator(ActorRight.Y, ActorRight.Z, ActorRight.X);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Actor Forward : %s"), *ActorRight.ToString()));
	//Super::AddControllerRollInput(NewRot.Roll);
	//Super::AddControllerPitchInput(NewRot.Pitch);
	//Super::AddControllerYawInput(NewRot.Yaw);
	//GetController()->AddActorLocalRotation(FVector(0.f, AxisValue, 0.f).ToOrientationQuat());
	//GetController()->SetControlRotation()
	/*
	if (FVector::DotProduct(GetActorUpVector(), FVector::UpVector) > 0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(-AxisValue, 0, 0));
		//Super::AddControllerPitchInput(AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < -0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(AxisValue, 0, 0));

		//Super::AddControllerPitchInput(-AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::RightVector) > 0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(0, -AxisValue, 0));

		//Super::AddControllerYawInput(AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::RightVector) < -0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(0, AxisValue, 0));

		//Super::AddControllerYawInput(-AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::ForwardVector) > 0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(0, 0, -AxisValue));

		//Super::AddControllerRollInput(AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::ForwardVector) < -0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(0, 0, AxisValue));

		//Super::AddControllerRollInput(-AxisValue);
	}*/

	Super::AddControllerPitchInput(AxisValue);
	if (AxisValue != 0)
	{
		TimeAtLastMoveMouse = GetWorld()->GetTimeSeconds();
	}
}

void ATPPawn::AddControllerYawInput(float AxisValue)
{
	/*FVector InputVec = FVector(0.f, 0.f, AxisValue);
	FVector NewInputVec = GetActorUpVector().Rotation().RotateVector(InputVec);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Actor Up : %s"), *GetActorUpVector().Rotation().ToString()));
	//Super::AddControllerRollInput(NewInputVec.X);
	Super::AddControllerPitchInput(NewInputVec.Y);
	Super::AddControllerYawInput(NewInputVec.Z);
	FVector ActorUp = GetActorUpVector() * AxisValue;
	FRotator NewRot = FRotator::FRotator(ActorUp.Y, ActorUp.Z, 0.f);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Actor Up : %s"), *ActorUp.ToString()));
	Super::AddControllerRollInput(NewRot.Roll);
	Super::AddControllerPitchInput(NewRot.Pitch);
	Super::AddControllerYawInput(NewRot.Yaw);
	*/
	/*
	if (FVector::DotProduct(GetActorUpVector(), FVector::UpVector) > 0.95f)
	{

		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(0, AxisValue, 0));
		//Super::AddControllerYawInput(AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < -0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(0, -AxisValue, 0));
		//Super::AddControllerYawInput(-AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::RightVector) > 0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(AxisValue, 0, 0));
		//Super::AddControllerRollInput(AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::RightVector) < -0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(-AxisValue, 0, 0));
		//Super::AddControllerRollInput(-AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::ForwardVector) > 0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(0, 0, AxisValue));
		//Super::AddControllerPitchInput(AxisValue);
	}
	else if (FVector::DotProduct(GetActorUpVector(), FVector::ForwardVector) < -0.95f)
	{
		GetController()->SetControlRotation(GetController()->GetControlRotation() + FRotator(0, 0, -AxisValue));
		//Super::AddControllerPitchInput(-AxisValue);
	}*/

	Super::AddControllerYawInput(AxisValue);
	if (AxisValue != 0)
	{
		TimeAtLastMoveMouse = GetWorld()->GetTimeSeconds();
	}
}

void ATPPawn::JumpPressed()
{
	if (bOnGround)
	{
		StdPrint(FString::Printf(TEXT("Jump Pressed")));
		TimeJumpPressed = GetWorld()->GetTimeSeconds();
		// Trigger lowering and shaking animation
	}

}

void ATPPawn::JumpReleased()
{
	if (bOnGround)
	{
		float jumpPower = 1000.0f;
		jumpPower = MaxJumpForce * FMath::Clamp(GetWorld()->GetTimeSeconds() - TimeJumpPressed, 0.0f, MaxJumpTime) / MaxJumpTime;



		Capsule->AddImpulse(GetActorUpVector() * jumpPower);

		Capsule->AddImpulse(Capsule->GetMass() * RealSpeed);

		//GetCharacterMovement()->GravityScale = 0.0f;
		// Stop lowering and shaking animation.
	}
}

void ATPPawn::SprintPressed()
{
	StdPrint(FString::Printf(TEXT("PRESSED SPRINT")));
	if (FVector::DotProduct(RealSpeed, GetActorForwardVector()) > .9f)
	{
		StdPrint(FString::Printf(TEXT("STARTED SPRINTING")));
		bSprinting = true;
	}
}

void ATPPawn::SprintReleased()
{
	bSprinting = false;
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

void ATPPawn::StdPrint(const FString message)
{
	if (true)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, message);
}
