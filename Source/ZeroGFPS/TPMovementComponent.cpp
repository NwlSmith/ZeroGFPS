// Fill out your copyright notice in the Description page of Project Settings.


#include "TPMovementComponent.h"

void UTPMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Make sure that everything is still valid, and that we are allowed to move.
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	AActor* Actor = GetOwner(); //->GetComponentByClass(UPrimitiveComponent::StaticClass());
	ATPPawn*  Pawn = Cast<ATPPawn>(Actor);


	// Get (and then clear) the movement vector that we set in ACollidingPawn::Tick
	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * Pawn->CalculateHorizontalMovementValue();
	//Pawn->StdPrint(FString::Printf(TEXT("DesiredMovement: %s"), *DesiredMovementThisFrame.ToString()));
	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		// If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}
		
	}
}
