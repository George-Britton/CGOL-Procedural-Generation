// Fill out your copyright notice in the Description page of Project Settings.


#include "Sea.h"

// Sets default values
ASea::ASea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SeaComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Sea Component"));
	BeachComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Beach Component"));
	SeaComponent->SetupAttachment(this->RootComponent);
	BeachComponent->SetupAttachment(this->RootComponent);
}

// Used to set the variables from external actors
void ASea::ReceiveCreateSea(UStaticMesh* InSeaMesh, UStaticMesh* InBeachMesh, float InSeaDistance, int32 InSeaHeight, int32 InSeaWidth, int32 InBeachWidth)
{
	// We first set the parameters from the function call
	SeaMesh = InSeaMesh;
	BeachMesh = InBeachMesh;
	SeaDistance = InSeaDistance;
	SeaHeight = InSeaHeight;
	SeaWidth = InSeaWidth;
	BeachWidth = InBeachWidth;

	// Then we initiate the components to own their meshes
	SeaComponent->SetStaticMesh(SeaMesh);
	BeachComponent->SetStaticMesh(BeachMesh);
	
	// And then we generate the sea
	CreateSea();
}

// Creates the sea using user-defined parameters
void ASea::CreateSea()
{
	// First we move the sea to be running past the city with the user-defined distances
	float TileWidth = SeaMesh->GetBoundingBox().GetSize().X;
	FVector RelativePos = FVector(-SeaDistance, 5.f, -0.25f * SeaHeight * TileWidth);
	this->SetActorRelativeLocation(RelativePos);

	// This is used to measure the total width of the entire tile count
	int32 TotalWidth = SeaWidth + BeachWidth;
	// Then we start looping through the sea population count
	for (int32 Tile = 0; Tile < TotalWidth * SeaHeight; ++Tile)
	{
		// We create a transform that holds the tile placement parameters
		FTransform TileTransform;
		TileTransform.SetLocation(FVector((Tile % TotalWidth) * TileWidth, FMath::FloorToInt(Tile / SeaHeight) * TileWidth, 0));

		// If the tile is further than the beach, place a sea tile, otherwise place a beach tile
		if (Tile % TotalWidth <= SeaWidth)
		{
			SeaComponent->AddInstance(TileTransform);
		}else BeachComponent->AddInstance(TileTransform);
	}	
}