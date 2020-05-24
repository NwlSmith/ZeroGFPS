// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPawn.h"

// Sets default values
AFPPawn::AFPPawn()
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

	PrevNormal = FVector(0.0f, 0.0f, 1.0f);
}

void AFPPawn::SetupCapsule()
{
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("Capsule")));
	Capsule->InitCapsuleSize(40.0f, 96.0f);
	Capsule->SetSimulatePhysics(true);
	Capsule->SetEnableGravity(false);
	Capsule->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	RootComponent = Capsule;
}

void AFPPawn::SetupCamera()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(FName(TEXT("Camera")));
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(-20.0f, 1.75f, 64.0f));
	Camera->bUsePawnControlRotation = true;
}

void AFPPawn::SetupMesh()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("SkeletalMesh")));
	SkeletalMesh->SetOwnerNoSee(true);
	SkeletalMesh->SetupAttachment(RootComponent);
}

void AFPPawn::SetupMovementComponent()
{
	// Create an instance of our movement component, and tell it to update the root.
	MovementComponent = CreateDefaultSubobject<UFPMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void AFPPawn::BeginPlay()
{
	Super::BeginPlay();

	Wrld = GetWorld();
}

// Called to bind functionality to input
void AFPPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	// Horizontal movement
	PlayerInputComponent->BindAxis("Forward", this, &AFPPawn::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &AFPPawn::MoveRight);

	// Mouse Axis Binding
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	// Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPPawn::JumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPPawn::JumpReleased);

	// Simple Action
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPPawn::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AFPPawn::InteractReleased);
}

// Called every frame
void AFPPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if floating,
	// get velocity vector,
	// linetrace from character in dir of velocity
	// set target up to normal of hit

	FHitResult OutHit;
	FVector Start = GetActorLocation();

	FVector Direction = GetVelocity().GetSafeNormal();
	Direction = -GetActorUpVector();
	FVector End = ((Direction * 5000.f) + Start);
	FCollisionQueryParams CollisionParams;

	

	//GetCharacterMovement()->GravityScale = 0.0f;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{

		// need to calculate player's rotation in relation to the previous normal, and apply that to its rotation in relation to the new normal
		// set body rotation, not camera?
		FRotator Rot = OutHit.ImpactNormal.Rotation();
		// FVector NewRightVector = -FVector::CrossProduct(OutHit.Normal, GetActorForwardVector());
		// FVector NewForwardVector = FVector::CrossProduct(OutHit.Normal, NewRightVector);
		FVector vec = FVector::CrossProduct(GetActorRightVector(), OutHit.Normal);
		Rot = vec.Rotation();
		//GetController()->SetControlRotation(Rot);
		//SetActorRotation(Rot);
		//FaceRotation(Rot);

		OutHit.ImpactNormal.Rotation();
		/*
		FTransform
		(
			const FVector & InX,
			const FVector & InY,
			const FVector & InZ,
			const FVector & InTranslation
		)
		*/

		//AddActorWorldRotation(FQuat::FindBetweenNormals(PrevNormal, OutHit.Normal));
		// Somehow make the camera always rotate with the actor
		//FVector(0, Camera->GetRelativeRotation.FVector ToOrientationQuat();
		//Camera->SetRelativeRotation();

		bool WasOnGround = OnGround;

		OnGround = CheckIsOnGround();

		if (!WasOnGround && OnGround)
		{
			Rot = OutHit.ImpactNormal.Rotation();
			SetActorRotation(Rot);
		}

		if (!OnGround)
		{
			//Capsule->AddForce(OutHit.ImpactNormal * -1.0f * 9800.0f);
		}

		if (OnGround)
		{
			Capsule->AddForce(GetActorUpVector() * -1.0f * 98000.0f);
		}

		PrevNormal = OutHit.Normal;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Rot: %s"), *vec.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Normal: %s"), *OutHit.Normal.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Object: %s"), *OutHit.Actor->GetHumanReadableName()));


	}

	// otherwise, set actor rotation to normal


	// if not floating, make gravity just point downward?
	//if (CanJump())
	//{

	//}
}

UPawnMovementComponent* AFPPawn::GetMovementComponent() const
{
	return MovementComponent;
}

void AFPPawn::MoveForward(float AxisValue)
{
	if (AxisValue != 0)
	{
		//Capsule->AddForce(GetActorForwardVector() * MoveSpeed * 10.0f * Val);
		//AddMovementInput(GetActorForwardVector(), AxisValue, true);
		if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
		{
			float movementValue = CalculateHorizontalMovementValue();
			MovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
		}
	}
}

void AFPPawn::MoveRight(float AxisValue)
{
	if (AxisValue != 0)
	{
		//AddMovementInput(GetActorRightVector(), AxisValue);
		if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
		{
			MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
		}
	}
}

float AFPPawn::CalculateHorizontalMovementValue()
{
	float MoveVal = MaxSpeed;

	if (!OnGround) { MoveVal *= InAirMovementDampener; }

	return MoveVal;
}

void AFPPawn::JumpPressed()
{
	if (OnGround)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Jump Pressed")));
		//TimeJumpPressed = Wrld->GetTimeSeconds();
		// Trigger lowering and shaking animation
	}

}

void AFPPawn::JumpReleased()
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

bool AFPPawn::CheckIsOnGround()
{
	FHitResult OutHit;
	FVector Start = GetActorLocation() - GetActorUpVector() * Capsule->GetScaledCapsuleHalfHeight();
	FVector End = ((GetActorUpVector() * -100.0f) + Start);
	FCollisionQueryParams CollisionParams;

	//GetCharacterMovement()->GravityScale = 0.0f;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		return true;
	}
	return false;
}

void AFPPawn::NotifyHit
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
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	FRotator rot = FVector::CrossProduct(GetActorRightVector(), HitNormal).Rotation();
	//GetController()->SetControlRotation(rot);
	//GetCharacterMovement()->GravityScale = 1.0f;


	UE_LOG(LogTemp, Warning, TEXT("Actor was hit"));
}

void AFPPawn::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("E was pressed"));
	Capsule->AddForce(GetActorUpVector() * -1.0f * 9800000.0f);
}

void AFPPawn::InteractReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("E was released"));
}
