// Fill out your copyright notice in the Description page of Project Settings.


#include "Sea.h"

// Sets default values
ASea::ASea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Used to set the variables from external actors
void ASea::ReceiveCreateSea(UMaterial* InSeaMaterial, float InSeaDistance)
{
	// We first set the parameters from the function call
	SeaMaterial = InSeaMaterial;
	SeaDistance = InSeaDistance;

	// And then we generate the sea
	CreateSea();
}

// Creates the sea landscape using user-defined parameters
void ASea::CreateSea()
{
	// We create the sea component, and set the material to the user-defined parameter
	FActorSpawnParameters SpawnParams;
	SeaLandscape = GetWorld()->SpawnActor<ALandscape>(ALandscape::StaticClass(), this->GetTransform(), SpawnParams);
	SeaLandscape->LandscapeMaterial = SeaMaterial;
}
