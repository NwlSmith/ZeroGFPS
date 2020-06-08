// Fill out your copyright notice in the Description page of Project Settings.


#include "OrientationStaticMeshComponent.h"

APawn* UOrientationStaticMeshComponent::GetTargetPawn()
{
	return TargetPawn;
}

void UOrientationStaticMeshComponent::SetTargetPawn(APawn * NewTarget)
{
	TargetPawn = NewTarget;
}

/*
void UOrientationStaticMeshComponent::CalculateOrientation()
{
	UE_LOG(LogTemp, Warning, TEXT("Calculate Orientation has not been implemented in this object."));
}*/
