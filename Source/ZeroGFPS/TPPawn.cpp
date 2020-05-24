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
	Capsule->InitCapsuleSize(40.0f, 96.0f);
	Capsule->SetSimulatePhysics(true);
	Capsule->SetEnableGravity(false);
	Capsule->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	RootComponent = Capsule;
}

void ATPPawn::SetupCamera()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName(TEXT("SpringArm")));
	SpringArm->SetupAttachment(RootComponent);
	//SpringArm->SetRelativeLocation(FVector(0.0f, 1.75f, 55.0f));


	Camera = CreateDefaultSubobject<UCameraComponent>(FName(TEXT("Camera")));
	Camera->SetupAttachment(SpringArm);
	//Camera->SetRelativeLocation(FVector(-20.0f, 1.75f, 64.0f));
	//Camera->bUsePawnControlRotation = true;
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
		}
	}
}

void ATPPawn::MoveRight(float AxisValue)
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

float ATPPawn::CalculateHorizontalMovementValue()
{
	float MoveVal = MaxSpeed;

	if (!OnGround) { MoveVal *= InAirMovementDampener; }

	return MoveVal;
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

