// Fill out your copyright notice in the Description page of Project Settings.


#include "CityGenerator.h"

// Sets default values
ACityGenerator::ACityGenerator()
{
	// We don't want the generator to tick as after the city generation it is static
	PrimaryActorTick.bCanEverTick = false;

	// We now construct the components and attach them all to the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	CityBuildingISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("City Building Instanced Static Mesh Component"));
	CityBoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("City Bounds Box"));
	CityBuildingISMComponent->SetupAttachment(this->RootComponent);
	CityBoundsBox->SetupAttachment(this->RootComponent);
}

// Called whenever a value changes
void ACityGenerator::OnConstruction(const FTransform& Transform)
{
	// Sets the static meshes to be instanced in the city
	if (CityBuilding) 
	{
		CityBuildingISMComponent->SetStaticMesh(CityBuilding);
		BuildingWidth = CityBuilding->GetBoundingBox().GetSize().X;
	}

	// We create the cell life rules array
	if (CellLifeRules.Num() == 0) CellLifeRules.Init(FCellLifeRule::FCellLifeRule(), 9);

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

	// Debug print strings
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, "Rows: " + FString::FromInt(Rows));
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, "Columns: " + FString::FromInt(Columns));
	
	// Now we generate a start and end position for the player's path by using the scale of the city, and choosing
	// distant points. Our StartingPosition is derived by taking a random point in the grid,
	// and the EndingPosition is the flip opposite point to the starting position.
	// We loop through the do while loop to make sure that the start/end positions aren't stuck in the boarder
	/*do
	{
		StartingPosition = FMath::RandRange(((Rows * Columns) - 1) / 3, ((Rows * Columns) - 1) / 2);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::FromInt());
	}
	while (StartingPosition >= Columns * (BoarderWidth + 1) && StartingPosition % Columns != Columns - (BoarderWidth + 1)
			&& StartingPosition < Columns * (Rows - (BoarderWidth + 1)) && StartingPosition % Columns > BoarderWidth);
	*/
	EndingPosition = ((Rows * Columns) - 1) - StartingPosition;
	PopulationGrid[StartingPosition] = false;
	
	// We then want to generate a random city
	GenerateCity();
	
	// We then generate the city, and declare it ready for the player
	if (BuildCity())
	{
		OnCityReady.Broadcast();
	}
}

// Called to create a randomly generated city
void ACityGenerator::GenerateCity()
{
	// First we create a holder array to help evolve the PopulationGrid
	TArray<bool> TemporaryArray;
	TemporaryArray.Init(false, PopulationGrid.Num());

	// We get a neighbour array
	TArray<int32> NeighbourArray = CreateNeighbourArray();
	
	// Next we loop a user-defined number of times, until we're satisfied with our city
	for(int32 GenerationLooper = 0; GenerationLooper < CityEvolutionGenerations; ++GenerationLooper)
	{
		// We loop through every cell of our grid
		for(int32 Cell = 0; Cell < PopulationGrid.Num(); ++Cell)
		{
			// We count how many neighbours are alive
			int32 LivingNeighbours = CountLivingNeighbours(Cell, NeighbourArray);
			
			if (IsWithinBorder(Cell)) TemporaryArray[Cell] = true; // Set borders to true
			else // We use the amount of alive neighbours as the index for CellLifeRules, and check what should happen to this cell
			{
				if (PopulationGrid[Cell]) TemporaryArray[Cell] = CellLifeRules[LivingNeighbours].AliveCellStaysAlive;
				else TemporaryArray[Cell] = CellLifeRules[LivingNeighbours].DeadCellBecomesLive;
			}
		}

		// We set the PopulationGrid to be equal to our new modified array
		PopulationGrid = TemporaryArray;
		TemporaryArray.Empty();
		TemporaryArray.Init(false, PopulationGrid.Num());
	}
	
}

// Creates an array that stores the relative addresses of a cell's neighbours
TArray<int32> ACityGenerator::CreateNeighbourArray()
{
	
	TArray<int32> RelativeNeighbourAddresses; // Create the variable
	RelativeNeighbourAddresses.Init(-1, 1); // Left
	RelativeNeighbourAddresses.Add(1); // Right
	RelativeNeighbourAddresses.Add(Columns); // Down
	RelativeNeighbourAddresses.Add(-Columns); // Up
	RelativeNeighbourAddresses.Add(Columns - 1); // Top left
	RelativeNeighbourAddresses.Add(Columns + 1); // Top right
	RelativeNeighbourAddresses.Add(-Columns - 1); // Bottom left
	RelativeNeighbourAddresses.Add(-Columns + 1); // Bottom Right

	return RelativeNeighbourAddresses;
}


// Counts the amounts of 'True' neighbours a grid cell has
int32 ACityGenerator::CountLivingNeighbours(int32 Cell, TArray<int32> RelativeNeighbours)
{
	// We set the current number of alive neighbours to 0
	int32 LivingNeighbours = 0;

	// Then we loop through the neighbour locations and count how many are alive
	for (int32 NeighbourLooper = 0; NeighbourLooper < 8; ++NeighbourLooper)
	{
		if (IsWithinBorder(Cell + RelativeNeighbours[NeighbourLooper])) { ++LivingNeighbours; continue; }
		if (PopulationGrid[Cell + RelativeNeighbours[NeighbourLooper]]) ++LivingNeighbours;
	}

	// We then return the results
	return LivingNeighbours;
}

// Returns whether or not the cell provided is in the border
bool ACityGenerator::IsWithinBorder(int32 Cell)
{
	// If the cell is in the top border return true
	if (Cell < Columns * BorderWidth) return true;
	// If the cell is in the bottom border return true
	if (Cell > (PopulationGrid.Num() - 1) - Columns * BorderWidth) return true;
	// If the cell is in the left border return true
	if (Cell % Columns <= BorderWidth - 1) return true;
	// If the cell is in the right border return true
	if (Cell % Columns > Columns - (BorderWidth + 1)) return true;

	// Else return false
	return false;
}


// Called to populate the world
bool ACityGenerator::BuildCity()
{
	// We now loop through the population grid and put a building in each 'true' square
	for (int32 GridSquare = 0; GridSquare < PopulationGrid.Num(); ++GridSquare)
	{
		// We find out whether we need to add a building
		if (PopulationGrid[GridSquare])
		{
			// This is the building width that is used to space out the grid
			FTransform GridTransform;

			// We randomise the rotation so the landscape varies
			GridTransform.SetRotation(FRotator(0.f, FMath::RandRange(0, 3) * 90, 0.f).Quaternion());

			// We derive the location from the modulus of the rows and columns, against the grid position
			GridTransform.SetLocation(FVector((GridSquare % Columns) * BuildingWidth,
				FMath::FloorToInt(GridSquare / Columns) * BuildingWidth,
				0.f));
			
			// Then we add the building transform to the ISM instance array
			CityBuildingISMComponent->AddInstance(GridTransform);
		}
		
	}
	return true;
}