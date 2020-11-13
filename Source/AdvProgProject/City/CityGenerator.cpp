// Fill out your copyright notice in the Description page of Project Settings.


#include "CityGenerator.h"
#include "Helicopter.h"

// Sets default values
ACityGenerator::ACityGenerator()
{
	// We don't want the generator to tick as after the city generation it is static
	PrimaryActorTick.bCanEverTick = false;

	// We now construct the components and attach them all to the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	CityBuildingISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("City Building Instanced Static Mesh Component"));
	CityBuildingISMComponent->SetupAttachment(this->RootComponent);
}

// Called whenever a value changes
void ACityGenerator::OnConstruction(const FTransform& Transform)
{
	// We clamp the border width to be a minimum of 0
	BorderWidth = FMath::Clamp(BorderWidth, 0, BorderWidth);
	
	// Sets the static meshes to be instanced in the city
	if (CityBuilding) CityBuildingISMComponent->SetStaticMesh(CityBuilding);

	// We create the cell life rules array
	if (CellLifeRules.Num() == 0) CellLifeRules.Init(FCellLifeRule::FCellLifeRule(), 9);
}

// Called when the game starts or when spawned
void ACityGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Derives the size of the city grid by dividing the box bounds by the size of the building
	PopulationGrid.Init(true, Rows * Columns);

	// Here we keep generating cities until one is walkable
	do
	{
		FindEnds();
		// We generate random noise for the first cell layout
		InitialiseGrid();
		// We then want to generate a random city
		GenerateCity();
	} while (!IsPathWalkable()); // We check our start and end position are connected	
	
	// We then generate the city, and declare it ready for the player
	if (BuildCity())
	{
		OnCityReady.Broadcast();
	}
}

// Finds a suitable start and end cell
void ACityGenerator::FindEnds()
{
	// Now we generate a start and end position for the player's path by using the scale of the city, and choosing
	// distant points. We loop through the do while loop to make sure that the start/end positions aren't stuck in the boarder
	do
	{
		// Our starting cell is somewhere in the first quarter of the grid
		StartingPosition = FMath::RandRange(0, FMath::FloorToInt(PopulationGrid.Num() / 4));
		// Our ending cell is somewhere in the last quarter of the grid
		EndingPosition = FMath::RandRange(FMath::FloorToInt(PopulationGrid.Num() * 0.75), PopulationGrid.Num());
	} while (IsWithinBorder(StartingPosition) || IsWithinBorder(EndingPosition));
}

// Initialises the population grid to be evolved
void ACityGenerator::InitialiseGrid()
{
	// We loop through the whole grid
	for(int32 CellNumber = 0; CellNumber < PopulationGrid.Num(); ++CellNumber)
	{
		// We check the cell isn't a boarder cell, and use RNG to check our chance of cell being dead
		if (!IsWithinBorder(CellNumber) && FMath::RandRange(0, 99) < CellLifePercent)
			PopulationGrid[CellNumber] = false;
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
	// We then make sure the start and end cells are open
	PopulationGrid[StartingPosition] = false;
	PopulationGrid[EndingPosition] = false;
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
		// If it's within the border, add 1 and continue
		if (IsWithinBorder(Cell + RelativeNeighbours[NeighbourLooper])) { ++LivingNeighbours; continue; }
		// If it's within the bounds of the city, and the neighbour is alive, add 1
		if (!IsOutOfBounds(Cell + RelativeNeighbours[NeighbourLooper]))
			if (PopulationGrid[Cell + RelativeNeighbours[NeighbourLooper]]) ++LivingNeighbours;
	}

	// We then return the results
	return LivingNeighbours;
}

// Returns whether or not the cell provided is in the border
bool ACityGenerator::IsWithinBorder(int32 Cell)
{
	// if there is no border, return false
	if (BorderWidth == 0) return false;
	
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
// Return whether or not the cell is out of bounds
bool ACityGenerator::IsOutOfBounds(int32 Cell)
{
	if (Cell < 0 || Cell >= PopulationGrid.Num()) return true;
	return false;
}

// Checks that there is a walkable path for the player
bool ACityGenerator::IsPathWalkable()
{
	// First we copy the population grid so we can fill it as we go without ruining it
	FloodArray = PopulationGrid;
	// We initiate the connected cell queue with the neighbours of the starting position
	TArray<int32> ConnectedCells = FloodFill(StartingPosition);
	// We set a flag for whether the path is walkable
	bool IsWalkable = false;

	// We keep looping through the connected cell queue
	while (true)
	{
		// If we run out of cells to connect, we have no walkable path, so we break and return
		if (ConnectedCells.Num() == 0) break;

		int32 NextCell = ConnectedCells.Pop();
		// If we're on the ending cell, we have a walkable path, so we set IsWalkable to true, and break
		if (NextCell == EndingPosition)
		{
			IsWalkable = true;
			break;
		}
		
		// We then go to the next cell in the queue and add their open neighbours to the queue
		ConnectedCells.Append(FloodFill(NextCell));
	}

	// We return whether or not there is a walkable path
	return IsWalkable;
}
TArray<int32> ACityGenerator::FloodFill(int32 Cell)
{
	// We create an array to fill with open neighbours
	TArray<int32> ConnectedCells;
	if (!IsOutOfBounds(Cell - 1))
		if (FloodArray[Cell - 1] == false) { ConnectedCells.Add(Cell - 1); FloodArray[Cell - 1] = true; } // Left
	if (!IsOutOfBounds(Cell + 1))
		if (FloodArray[Cell + 1] == false) { ConnectedCells.Add(Cell + 1); FloodArray[Cell + 1] = true; } // Right
	if (!IsOutOfBounds(Cell - Columns))
		if (FloodArray[Cell - Columns] == false) { ConnectedCells.Add(Cell - Columns); FloodArray[Cell - Columns] = true; } // Up
	if (!IsOutOfBounds(Cell + Columns))
		if (FloodArray[Cell + Columns] == false) { ConnectedCells.Add(Cell + Columns); FloodArray[Cell + Columns] = true; } // Down

	return ConnectedCells;
}

// Called to populate the world
bool ACityGenerator::BuildCity()
{
	// The width of the building mesh, used to align them to grid
	float BuildingWidth = CityBuilding->GetBoundingBox().GetSize().X;
	
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
	CreateEnding(BuildingWidth);
	return true;
}

// Sets up the helicopter ending space
void ACityGenerator::CreateEnding(float BuildingWidth)
{
	// We first initialise the ending location
	FActorSpawnParameters HeliSpawnParams;
	FVector EndSpawnLocation = this->GetActorLocation();

	// We then add the relative position from the index and column count
	EndSpawnLocation.X += (EndingPosition % Columns) * BuildingWidth;
	EndSpawnLocation.Y += FMath::FloorToInt(EndingPosition / Columns) * BuildingWidth;
	EndSpawnLocation.Z += HelicopterMesh->GetBounds().BoxExtent.Z / 2;

	// We put the helicopter there and set the mesh
	AHelicopter* Helicopter = GetWorld()->SpawnActor<AHelicopter>(AHelicopter::StaticClass(), EndSpawnLocation, FRotator::ZeroRotator, HeliSpawnParams);
	Helicopter->InitialiseMeshAndBounds(HelicopterMesh);
}
