// Fill out your copyright notice in the Description page of Project Settings.


#include "Helicopter.h"


// Sets default values
AHelicopter::AHelicopter()
{
	// Set this actor to not tick every frame.
	PrimaryActorTick.bCanEverTick = false;

	// Here we create the component variables
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	HelicopterMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helicopter Mesh Component"));
	EndOverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("End Overlap Box"));
	HelicopterMeshComponent->SetupAttachment(this->RootComponent);
	EndOverlapBox->SetupAttachment(this->RootComponent);
}

// Called when another actor overlaps with this actor
void AHelicopter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor->GetName() == "Player") OnPlayerReachEnd.Broadcast();
}

// Initialises the box around the added mesh
void AHelicopter::InitialiseMeshAndBounds(USkeletalMesh* Mesh)
{
	HelicopterMeshComponent->SetSkeletalMesh(Mesh);
	EndOverlapBox->SetBoxExtent(Mesh->GetBounds().BoxExtent * RelativeEndBoxSize);
	this->SetActorRotation(FRotator(0, 45, 0));
}
