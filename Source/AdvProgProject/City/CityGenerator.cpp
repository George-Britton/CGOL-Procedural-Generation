// Fill out your copyright notice in the Description page of Project Settings.


#include "CityGenerator.h"

// Sets default values
ACityGenerator::ACityGenerator()
{
	// We don't want the generator to tick as after the city generation it is static
	PrimaryActorTick.bCanEverTick = true;

	// We now construct the components and attach them all to the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	CityISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("City Instanced Static Mesh Component"));
	CityBoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("City Bounds Box"));
	CityISMComponent->SetupAttachment(this->RootComponent);
	CityBoundsBox->SetupAttachment(this->RootComponent);
}

// Called whenever a value changes
void ACityGenerator::OnConstruction(const FTransform& Transform)
{
	// Sets the static mesh to be instanced in the city
	if (CityBuilding) 
	{
		CityISMComponent->SetStaticMesh(CityBuilding);
		BuildingWidth = CityBuilding->GetBoundingBox().GetSize().X;
	}

	// Here we clamp the beginning of the box to one corner
	CityBoundsBox->SetRelativeLocation(FVector(CityBoundsBox->GetScaledBoxExtent().X, CityBoundsBox->GetScaledBoxExtent().Y, 0));
}

// Called when the game starts or when spawned
void ACityGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Derives the size of the city grid by dividing the box bounds by the size of the building
	Columns = FMath::FloorToInt(CityBoundsBox->GetScaledBoxExtent().X / CityBuilding->GetBounds().GetBox().GetSize().X);
	Rows = FMath::FloorToInt(CityBoundsBox->GetScaledBoxExtent().Y / CityBuilding->GetBounds().GetBox().GetSize().Y);
	PopulationGrid.Init(true, Rows * Columns);

	// Now we generate a start and end position for the player's path by using the scale of the city, and choosing
	// distant points. Our StartingPosition is derived by taking a random point in the grid,
	// and the EndingPosition is the flip opposite point to the starting position.
	StartingPosition = FMath::RandRange((Columns * BoarderWidth) + BoarderWidth, ((Rows * Columns) - 1) / 2);
	EndingPosition = ((Rows * Columns) - 1) - StartingPosition;
	PopulationGrid[StartingPosition] = false;

	// Debug print strings
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, "Rows: " + FString::FromInt(Rows));
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, "Columns: " + FString::FromInt(Columns));
	
	// We then want to generate a random city
	DrunkardWalk();
	
	// We then generate the city, and declare it ready for the player
	if (GenerateCity())
	{
		OnCityReady.Broadcast();
	}
}

// Called to implement the drunkard walk fractional brownian motion algorithm
void ACityGenerator::DrunkardWalk()
{
	// We start our 'drunkard' on the starting position...
	int32 CurrentPosition = StartingPosition;

	// ... and let him aimlessly wander until he reaches the end
	// (which means we turn each stepped onto square to false, meaning "don't put a building here")
	while(PopulationGrid[EndingPosition])
	{
		CurrentPosition = Step(CurrentPosition);
		if (PopulationGrid[CurrentPosition]) PopulationGrid[CurrentPosition] = false;
	}
}
int32 ACityGenerator::Step(int32 DrunkardPosition)
{
	// We randomly select a direction for the drunkard to step
	EDirection NextStep = EDirection(FMath::RandRange(0, 3));
	
	// We then use a switch to implement a step in that direction,
	// by turning the array into a 2d grid, and stepping through it.
	// This implementation means that we always have a BoarderWidth-layer
	// boarder of buildings around the outside, and a guaranteed path
	// from start to finish
	switch(NextStep)
	{
	case EDirection::NORTH:	if (DrunkardPosition >= Columns * (BoarderWidth + 1)) { return (DrunkardPosition - Columns); }
							break;
	case EDirection::EAST:	if (DrunkardPosition % Columns != Columns - (BoarderWidth + 1)) { return (DrunkardPosition + 1); }
							break;
	case EDirection::SOUTH: if (DrunkardPosition < Columns * (Rows - (BoarderWidth + 1))) { return (DrunkardPosition + Columns); }
							break;
	case EDirection::WEST:	if (DrunkardPosition % Columns > BoarderWidth) { return (DrunkardPosition - 1); }
							break;
	default: break;
	}
	
	// Catch for if the drunkard cannot move in the intended direction
	return DrunkardPosition;
}

// Called to populate the world
bool ACityGenerator::GenerateCity()
{
	// We now loop through the population grid and put a building in each 'true' square
	for (int32 GridSquare = 0; GridSquare < PopulationGrid.Num(); ++GridSquare)
	{
		if (PopulationGrid[GridSquare])
		{
			// This is the building width that is used to space out the grid
			FTransform BuildingTransform;

			// We randomise the rotation so the landscape varies
			BuildingTransform.SetRotation(FRotator(0.f, FMath::RandRange(0, 3) * 90, 0.f).Quaternion());
			
			// We derive the location from the modulus of the rows and columns, against the grid position
			BuildingTransform.SetLocation(FVector((GridSquare % Columns) * BuildingWidth,
				FMath::FloorToInt(GridSquare / Columns) * BuildingWidth,
				0.f));

			// Then we add the building transform to the ISM instance array
			CityISMComponent->AddInstance(BuildingTransform);
		}
	}
	CityISMComponent->MarkRenderStateDirty();
	return true;
}