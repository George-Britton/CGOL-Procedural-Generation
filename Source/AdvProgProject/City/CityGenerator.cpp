// Fill out your copyright notice in the Description page of Project Settings.


#include "CityGenerator.h"

// Sets default values
ACityGenerator::ACityGenerator()
{
	// We don't want the generator to tick as after the city generation it is static
	PrimaryActorTick.bCanEverTick = false;

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
	if (CityBuilding) CityISMComponent->SetStaticMesh(CityBuilding);

	// Here we clamp the beginning of the box to one corner
	CityBoundsBox->SetRelativeLocation(FVector(CityBoundsBox->GetScaledBoxExtent().X, CityBoundsBox->GetScaledBoxExtent().Y, 0));
}

// Called when the game starts or when spawned
void ACityGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Derives the size of the city grid by dividing the box bounds by the size of the building
	Rows = FMath::FloorToInt(CityBoundsBox->GetScaledBoxExtent().X / CityBuilding->GetBounds().GetBox().GetSize().X);
	Columns = FMath::FloorToInt(CityBoundsBox->GetScaledBoxExtent().Y / CityBuilding->GetBounds().GetBox().GetSize().Y);
	PopulationGrid.Init(true, Rows * Columns);
	PopulationGrid[StartingPosition] = false;

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
		PopulationGrid[CurrentPosition] = false;
	}
}
int32 ACityGenerator::Step(int32 DrunkardPosition)
{
	// We randomly select a direction for the drunkard to step
	EDirection NextStep = EDirection(FMath::RandRange(0, 3));

	// We then use a switch to implement a step in that direction,
	// by turning the array into a 2d grid, and stepping through it
	switch(NextStep)
	{
	case EDirection::NORTH: if (DrunkardPosition < Columns) { break; }
							return (DrunkardPosition - Columns);
	case EDirection::EAST:	if (DrunkardPosition % Columns == Columns - 1) { break; }
							return (DrunkardPosition + 1);
	case EDirection::SOUTH: if (DrunkardPosition > Columns * (Rows - 1)) { break; }
							return (DrunkardPosition + Columns);
	case EDirection::WEST:	if (DrunkardPosition % Columns == 0) { break; }
							return (DrunkardPosition - 1);
	default: break;
	}

	// debug catch, should never occur
	return StartingPosition;
}

// Called to populate the world
bool ACityGenerator::GenerateCity()
{
	// We now loop through the population grid and put a building in each 'true' square
	for (int32 GridSquare = 0; GridSquare < PopulationGrid.Num(); ++GridSquare)
	{
		FTransform BuildingTransform;
		float BuildingWidth = CityBuilding->GetBounds().GetBox().GetSize().X;
		
		// We derive the location from the modulus of the rows and columns, against the grid position
		BuildingTransform.SetLocation(FVector((GridSquare - (GridSquare % Columns)) * BuildingWidth,
													(FMath::FloorToInt(GridSquare / Columns)) * BuildingWidth,
													0.f));

		// We randomise the rotation so the landscape varies
		BuildingTransform.SetRotation(FRotator(0.f, FMath::RandRange(0, 3) * 90, 0.f).Quaternion());

		// Then we add the building transform to the ISM instance array
		CityISMComponent->AddInstance(BuildingTransform);
	}

	return true;
}