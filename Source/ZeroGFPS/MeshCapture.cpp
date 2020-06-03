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
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Mesh")));
	StaticMesh->SetupAttachment(ThisRoot);
}

void AMeshCapture::SetupCapture()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName(TEXT("SpringArm")));
	SpringArm->SetupAttachment(ThisRoot);

	Capture = CreateDefaultSubobject<USceneCaptureComponent2D>(FName(TEXT("Capture")));
	Capture->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AMeshCapture::BeginPlay()
{
	Super::BeginPlay();
	
	//PlayerPawn = 
}

// Called every frame
void AMeshCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	FTransform NewTrans = FTransform();
	//PlayerPawn

}

