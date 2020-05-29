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
	SkeletalMesh->SetupAttachment(RootComponent);
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

		//StdPrint(FString::Printf(TEXT("Velocity: %s"), *GetVelocity().ToString()));
		
		UpdateRealSpeed();

		if (OnGround)
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
	// Retrieve locations of Sockets on SkeletalMesh
	const FVector Center = SkeletalMesh->GetSocketLocation("center");
	const FVector Under = SkeletalMesh->GetSocketLocation("down");
	//const FVector Forward = SkeletalMesh->GetSocketLocation("forward"); // may remove
	const FVector ForwardBot = SkeletalMesh->GetSocketLocation("forwardBot");
	//const FVector Back = SkeletalMesh->GetSocketLocation("back"); // may remove
	const FVector BackBot = SkeletalMesh->GetSocketLocation("backBot");

	// Create HitResults to hold hit data.
	FHitResult HitDataD(ForceInit);  // Check downwards
	//FHitResult HitDataF(ForceInit);  // Check forwards
	FHitResult HitDataFB(ForceInit); // Check forward bottom
	//FHitResult HitDataB(ForceInit);  // Check back NEED TO CHECK IF MOVING BACK
	FHitResult HitDataBB(ForceInit); // Check back bottom
	FHitResult HitDataFU(ForceInit); // Check forward to under vector
	FHitResult HitDataBU(ForceInit); // Check back to under vector

	FCollisionQueryParams CollisionParams;

	// Perform line traces
	GetWorld()->LineTraceSingleByChannel(HitDataD, Center, Under, ECC_Visibility, CollisionParams);
	GetWorld()->LineTraceSingleByChannel(HitDataFB, Center, ForwardBot, ECC_Visibility, CollisionParams);
	GetWorld()->LineTraceSingleByChannel(HitDataFU, ForwardBot, Under, ECC_Visibility, CollisionParams);

	// Update OnGround
	GroundTransitionCalculations(HitDataD.GetActor() != nullptr);
	
	//FVector::DotProduct(MovementComponent->Velocity, GetActorForwardVector())

	// Firstly, keep the pawn upright.
	if (CheckIfCanTransitionToNewSurface(HitDataD))
	{
		StdPrint(FString::Printf(TEXT("DOWN hit name: %s"), *HitDataD.GetActor()->GetName()));
		StdPrint(FString::Printf(TEXT("Transitioning to DOWN vector")));
		TransitionToNewSurface(HitDataD);
	}
	// If there is a wall in front of the pawn, transition to it
	else if (CheckIfCanTransitionToNewSurface(HitDataFB))
	{
		StdPrint(FString::Printf(TEXT("ForwardBot hit name: %s"), *HitDataFB.GetActor()->GetName()));
		StdPrint(FString::Printf(TEXT("Transitioning to FORWARDBOTTOM vector")));
		TransitionToNewSurface(HitDataFB);
	}
	// If there is nothing in front of the player, and the player is on a ledge, transition to it.
	else if (!HitDataFB.GetActor() && CheckIfCanTransitionToNewSurface(HitDataFU))
	{
		StdPrint(FString::Printf(TEXT("Forward -> under hit name: %s"), *HitDataFU.GetActor()->GetName()));
		StdPrint(FString::Printf(TEXT("Transitioning to forward -> under vector")));
		TransitionToNewSurface(HitDataFU);
	}
	/*else if (HitDataBB.GetActor() && CheckIfNeedToTransitionToNewSurface(HitDataBB.Normal))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Back Bottom hit name: %s"), *HitDataBB.GetActor()->GetName()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Transitioning to BottomBack vector")));
		//TransitionToNewSurface(HitDataBB);
	}
	else if (HitDataFR.GetActor() && CheckIfNeedToTransitionToNewSurface(HitDataFR.Normal))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Front RIGHT hit name: %s"), *HitDataFR.GetActor()->GetName()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Transitioning to right vector")));
		//TransitionToNewSurface(HitDataFR);
	}*/
}

bool ATPPawn::CheckIfCanTransitionToNewSurface(FHitResult Hit)
{
	return Hit.GetActor() && CheckIfNeedToTransitionToNewSurface(Hit.Normal);
}

bool ATPPawn::CheckIfNeedToTransitionToNewSurface(FVector HitNormal)
{
	//StdPrint(FString::Printf(TEXT("Normal : %s Up : %s"), *HitNormal.ToString(), *GetActorUpVector().ToString()));
	//StdPrint(FString::Printf(TEXT("Dot : %f"), FVector::DotProduct(HitNormal, GetActorUpVector())));
	return FVector::DotProduct(HitNormal, GetActorUpVector()) < SurfaceTransitionThreshold;
}

void ATPPawn::TransitionToNewSurface(FHitResult Hit)
{
	//Grab a ref to the world here
	UWorld* world = GetWorld();

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
}

void ATPPawn::GroundTransitionCalculations(bool TraceHitObj)
{
	// If was on the ground last tick and is now not on the ground...
	if (OnGround && !TraceHitObj)
	{
		// Turn off gravity
		CurGravityScale = 0.0f;
		StdPrint(FString::Printf(TEXT("WAS ON GROUND AND NOW IS NOT")));
	}
	// If was NOT on ground last tick and is now on ground...
	else if (!OnGround && TraceHitObj)
	{
		// Turn on gravity and get rid of any velocity.
		CurGravityScale = GravityScale;
		Capsule->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Capsule->SetPhysicsAngularVelocity(FVector::ZeroVector);
		StdPrint(FString::Printf(TEXT("WAS NOT ON GROUND AND NOW IS")));
	}
	OnGround = TraceHitObj;
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
	if (OnGround)
		return;

	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	//TransitionToNewSurface(HitNormal);


	UE_LOG(LogTemp, Warning, TEXT("Actor was hit"));
}

void ATPPawn::MoveForward(float AxisValue)
{
	if (AxisValue != 0)
	{
		//Capsule->AddForce(GetActorForwardVector() * MoveSpeed * 10.0f * Val);
		//AddMovementInput(GetActorForwardVector(), AxisValue, true);
		if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
		{
			FVector movementDirection = GetActorForwardVector();
			movementDirection = FVector::VectorPlaneProject(GetControlRotation().Vector(), GetActorUpVector()).GetSafeNormal();
			MovementComponent->AddInputVector(movementDirection * AxisValue);

			FRotator NewRot = FMath::QInterpTo(GetActorRotation().Quaternion(), movementDirection.ToOrientationQuat(), 0.5f, .25f).Rotator();
			SetActorRotation(NewRot);
		}
	}
}

void ATPPawn::MoveRight(float AxisValue)
{
	if (AxisValue != 0)
	{
		// This will be commented out for now. In the future, I want the directional keys to make the player turn to move in that direction.
		if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
		{
			MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
		}/*
		FTransform NewTransform = GetTransform();

		FRotator rot = FRotator(0, AxisValue * RotationSpeed / 30.0f, 0);

		NewTransform.ConcatenateRotation(rot.Quaternion());
		NewTransform.NormalizeRotation();
		SetActorTransform(NewTransform);*/
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
		StdPrint(FString::Printf(TEXT("Jump Pressed")));
		TimeJumpPressed = GetWorld()->GetTimeSeconds();
		// Trigger lowering and shaking animation
	}

}

void ATPPawn::JumpReleased()
{
	if (OnGround)
	{
		float jumpPower = 1000.0f;
		jumpPower = MaxJumpForce * FMath::Clamp(GetWorld()->GetTimeSeconds() - TimeJumpPressed, 0.0f, MaxJumpTime) / MaxJumpTime;

		Capsule->AddImpulse(GetActorUpVector() * jumpPower);

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

void ATPPawn::StdPrint(FString message)
{
	if (true)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, message);
}