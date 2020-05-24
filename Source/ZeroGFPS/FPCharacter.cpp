// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacter.h"

// Sets default values
AFPCharacter::AFPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	UCapsuleComponent* cap = GetCapsuleComponent();

	cap->InitCapsuleSize(55.0f, 96.0f);

	Camera = CreateDefaultSubobject<UCameraComponent>(FName(TEXT("Camera")));
	Camera->SetupAttachment(cap);
	Camera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.0f));
	Camera->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void AFPCharacter::BeginPlay()
{
	Super::BeginPlay();
	Wrld = GetWorld();
}



// Called every frame
void AFPCharacter::Tick(float DeltaTime)
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
	//UE_LOG(LogTemp, Warning, TEXT("ticking"));

	GetCharacterMovement()->GravityScale = 0.0f;
	


	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		// set body rotation, not camera?
		FRotator Rot = OutHit.ImpactNormal.Rotation();
		FVector vec = FVector::CrossProduct(GetActorRightVector(), OutHit.Normal);
		Rot = vec.Rotation();
		//GetController()->SetControlRotation(Rot);
		SetActorRotation(Rot);

		GetCharacterMovement()->AddForce(OutHit.ImpactNormal * -1.0f * 980.0f);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Rot: %s"), *vec.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Normal: %s"), *OutHit.Normal.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Object: %s"), *OutHit.Actor->GetHumanReadableName()));

		
	}


	// otherwise, set actor rotation to normal


	// if not floating, make gravity just point downward?
	if (CanJump())
	{

	}
}

// Called to bind functionality to input
void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Horizontal movement
	PlayerInputComponent->BindAxis("Forward", this, &AFPCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &AFPCharacter::MoveRight);

	// Mouse Axis Binding
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	// Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPCharacter::JumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPCharacter::JumpReleased);

	// Simple Action
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPCharacter::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AFPCharacter::InteractReleased);
}

void AFPCharacter::MoveForward(float Val)
{
	if (Val != 0)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void AFPCharacter::MoveRight(float Val)
{
	if (Val != 0)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void AFPCharacter::JumpPressed()
{
	if (CanJump())
	{
		TimeJumpPressed = Wrld->GetTimeSeconds();
		// Trigger lowering and shaking animation
	}
	
}

void AFPCharacter::JumpReleased()
{
	if (CanJump())
	{
		float jumpPower = 1000.0f;
		//jumpPower = MaxJumpForce * FMath::Clamp(Wrld->GetTimeSeconds() - TimeJumpPressed, 0.0f, MaxJumpTime) / MaxJumpTime;

		LaunchCharacter(GetActorUpVector() * jumpPower, false, false);

		GetCharacterMovement()->GravityScale = 0.0f;
		// Stop lowering and shaking animation.
	}
}

void AFPCharacter::NotifyHit
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
	GetController()->SetControlRotation(rot);
	GetCharacterMovement()->GravityScale = 1.0f;

	
	UE_LOG(LogTemp, Warning, TEXT("Actor was hit"));
}

void AFPCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("E was pressed"));
}

void AFPCharacter::InteractReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("E was released"));
}
