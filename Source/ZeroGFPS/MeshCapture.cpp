// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshCapture.h"

// Sets default values
AMeshCapture::AMeshCapture()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetupRoot();
	SetupMesh();
	SetupCapture();
}

void AMeshCapture::SetupRoot()
{
	ThisRoot = CreateDefaultSubobject<USceneComponent>(FName(TEXT("Root")));
	RootComponent = ThisRoot;
}

void AMeshCapture::SetupMesh()
{
	DownMesh = CreateDefaultSubobject<UDownOrientationMeshComponent>(FName(TEXT("Down Mesh")));
	DownMesh->CastShadow = false;
	DownMesh->SetupAttachment(ThisRoot);
}

void AMeshCapture::SetupCapture()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName(TEXT("SpringArm")));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetVisibility(false);
	SpringArm->SetupAttachment(ThisRoot);

	Capture = CreateDefaultSubobject<USceneCaptureComponent2D>(FName(TEXT("Capture")));
	Capture->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AMeshCapture::BeginPlay()
{
	Super::BeginPlay();
	Capture->ShowOnlyActorComponents(this);

	APlayerController* APC = GetWorld()->GetFirstPlayerController();
	if (APC != nullptr)
	{
		DownMesh->SetTargetPawn(APC->GetPawn());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Actor is : %s"), *DownMesh->GetTargetPawn()->GetFName().ToString()));
	}
	
	//PlayerPawn = 
}

// Called every frame
void AMeshCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	DownMesh->CalculateOrientation();

}

