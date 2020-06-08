// Fill out your copyright notice in the Description page of Project Settings.


#include "DownOrientationMeshComponent.h"
#include "TPPawn.h"

void UDownOrientationMeshComponent::CalculateOrientation()
{
	//GetWorld()->
	//TargetPawn->GetControlRotation()
	//FRotator NewRot = /*FRotator::MakeFromEuler(FVector(180.f, 0.f, 0.f)) -*/ Cast<ATPPawn>(TargetPawn)->GetActorRotation();
	FRotator NewRot = Cast<ATPPawn>(TargetPawn)->GetControlRotation();
	//Cast<ATPPawn>(TargetPawn)->GetController()->GetTransformComponent();
	NewRot = -1 * Cast<ATPPawn>(TargetPawn)->GetController()->GetDesiredRotation();
	SetWorldRotation(NewRot);
}
