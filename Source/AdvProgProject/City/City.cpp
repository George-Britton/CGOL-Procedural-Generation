// Fill out your copyright notice in the Description page of Project Settings.


#include "City.h"
#include "Helicopter.h"

// Sets default values
ACity::ACity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// We now construct the components and attach them all to the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	CityBuildingISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("City Building Instanced Static Mesh Component"));
	RoadComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Road Component"));
	CityBuildingISMComponent->SetupAttachment(this->RootComponent);
	RoadComponent->SetupAttachment(this->RootComponent);
}

// Called to set the city parameters to the user-defined ones
void ACity::ReceiveCityParameters(int32 InCityEvolutions, TArray<FCellLifeRule> InLifeRules, int32 InLifePercent, int32 InBorderWidth, UStaticMesh* InCityBuilding, int32 InRows, int32 InColumns, TArray<UStaticMesh*> InPropArray, float InPropProbability, UStaticMesh* InRoadMesh, UMaterial* InRoadMaterial, USkeletalMesh* InHeliMesh, float InHeliScale)
{
	// We set the generation parameters to the user-defined
	CityEvolutionGenerations = InCityEvolutions;
	CellLifeRules = InLifeRules;
	CellLifePercent = InLifePercent;
	BorderWidth = InBorderWidth;
	CityBuilding = InCityBuilding;
	Rows = InRows;
	Columns = InColumns;
	PropArray = InPropArray;
	PropSpawnProbability = InPropProbability;
	RoadMesh = InRoadMesh;
	RoadMaterial = InRoadMaterial;
	HelicopterMesh = InHeliMesh;
	HelicopterScale = InHeliScale;

	// We then sanitize the input and filter it through to the required components
	SanitizeInput();
}

// Called to sanitize the input from the user
void ACity::SanitizeInput()
{
	// We create the cell life rules array
	if (CellLifeRules.Num() == 0) CellLifeRules.Init(FCellLifeRule::FCellLifeRule(), 9);
	
	// We clamp the border width to be a minimum of 0
	BorderWidth = FMath::Clamp(BorderWidth, 0, BorderWidth);

	// We also clamp the prop spawn percent to between 0 and 100
	PropSpawnProbability = FMath::Clamp(PropSpawnProbability, 0.f, 100.f);
	
	// Sets the static meshes to be instanced in the city
	if (CityBuilding) CityBuildingISMComponent->SetStaticMesh(CityBuilding);
	//if (RoadMesh) RoadComponent->SetStaticMesh(RoadMesh);
	//if (RoadMaterial) RoadComponent->SetMaterial(0, RoadMaterial);
	
	// We create the cell life rules array
	if (CellLifeRules.Num() == 0) CellLifeRules.Init(FCellLifeRule::FCellLifeRule(), 9);

	// We then create the city
	BeginCreateCity();
}

// Called to create the city
void ACity::BeginCreateCity()
{
	// We create the prop arrays for city debris population
	if (PropArray.Num()) CreatePropComponents();

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

	// We then populate the city
	BuildCity();
}

// GENERATION
// Called to create the city prop components
void ACity::CreatePropComponents()
{
	// We initiate the array to be as big as the prop array
	PropComponentArray.Init(nullptr, PropArray.Num());

	// Then for each entry, we construct a component and add it to the root
	for (int32 PropLooper = 0; PropLooper < PropArray.Num(); ++PropLooper)
	{
		PropComponentArray[PropLooper] = Cast<UInstancedStaticMeshComponent>(StaticConstructObject_Internal(UInstancedStaticMeshComponent::StaticClass(), this));
		PropComponentArray[PropLooper]->RegisterComponent();
		PropComponentArray[PropLooper]->SetupAttachment(this->RootComponent);
		PropComponentArray[PropLooper]->SetStaticMesh(PropArray[PropLooper]);
		PropComponentArray[PropLooper]->SetWorldLocation(this->GetActorLocation());
	}
}
// Finds a suitable start and end cell
void ACity::FindEnds()
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
void ACity::InitialiseGrid()
{
	// We loop through the whole grid
	for (int32 CellNumber = 0; CellNumber < PopulationGrid.Num(); ++CellNumber)
	{
		// We check the cell isn't a boarder cell, and use RNG to check our chance of cell being dead
		if (!IsWithinBorder(CellNumber) && FMath::RandRange(0, 99) < CellLifePercent)
			PopulationGrid[CellNumber] = false;
	}
}
// Called to create a randomly generated city
void ACity::GenerateCity()
{
	// First we create a holder array to help evolve the PopulationGrid
	TArray<bool> TemporaryArray;
	TemporaryArray.Init(false, PopulationGrid.Num());

	// We get a neighbour array
	TArray<int32> NeighbourArray = CreateNeighbourArray();

	// Next we loop a user-defined number of times, until we're satisfied with our city
	for (int32 GenerationLooper = 0; GenerationLooper < CityEvolutionGenerations; ++GenerationLooper)
	{
		// We loop through every cell of our grid
		for (int32 Cell = 0; Cell < PopulationGrid.Num(); ++Cell)
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
// Checks that there is a walkable path for the player
bool ACity::IsPathWalkable()
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
TArray<int32> ACity::FloodFill(int32 Cell)
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

// CELLULAR AUTOMATION
// Creates an array that stores the relative addresses of a cell's neighbours
TArray<int32> ACity::CreateNeighbourArray()
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
int32 ACity::CountLivingNeighbours(int32 Cell, TArray<int32> RelativeNeighbours)
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
bool ACity::IsWithinBorder(int32 Cell)
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
bool ACity::IsOutOfBounds(int32 Cell)
{
	if (Cell < 0 || Cell >= PopulationGrid.Num()) return true;
	return false;
}

// CITY POPULATION
// Called to populate the world
bool ACity::BuildCity()
{
	// The width of the building mesh, used to align them to grid
	float BuildingWidth = CityBuilding->GetBoundingBox().GetSize().X;

	// We now loop through the population grid and put a building in each 'true' square
	for (int32 GridSquare = 0; GridSquare < PopulationGrid.Num(); ++GridSquare)
	{
		FTransform GridTransform;

		// We derive the location from the modulus of the rows and columns, against the grid position
		GridTransform.SetLocation(FVector(((GridSquare % Columns) * BuildingWidth) + BuildingWidth / 2,
			(FMath::FloorToInt(GridSquare / Columns) * BuildingWidth) + BuildingWidth / 2,
			0.f));

		// We find out whether we need to add a building
		if (PopulationGrid[GridSquare]) PlaceBuilding(GridTransform, BuildingWidth);
		else
		{
			PlaceRoad(GridTransform, BuildingWidth);
			if (FMath::RandRange(0.f, 99.9f) < PropSpawnProbability && PropArray.Num()) PlaceProp(GridTransform, BuildingWidth);
		}
	}

	CreateHelicopter();
	return true;
}
void ACity::PlaceBuilding(FTransform PlacementTransform, float BuildingWidth)
{
	// We randomise the rotation so the landscape varies
	PlacementTransform.SetRotation(FRotator(0.f, FMath::RandRange(0, 3) * 90, 0.f).Quaternion());

	// Then we add the building transform to the ISM instance array
	CityBuildingISMComponent->AddInstance(PlacementTransform);
}
void ACity::PlaceProp(FTransform PlacementTransform, float BuildingWidth)
{
	// We randomly choose which prop will be placed
	int32 ChosenProp = FMath::RandRange(0, PropArray.Num() - 1);

	// We randomise the rotation so the props vary
	PlacementTransform.SetRotation(FRotator(0.f, FMath::RandRange(0, 359), 0.f).Quaternion());

	// TODO: Randomise positioning

	// We then place the prop
	PropComponentArray[ChosenProp]->AddInstance(PlacementTransform);
}
void ACity::PlaceRoad(FTransform PlacementTransform, float BuildingWidth)
{
	PlacementTransform.SetRotation(FRotator(0, FMath::RandRange(0, 3) * 90, 0).Quaternion());
	PlacementTransform.SetScale3D(FVector(RoadMesh->GetBoundingBox().GetSize().X / BuildingWidth));
	RoadComponent->AddInstance(PlacementTransform);
}

// Sets up the helicopter ending space
void ACity::CreateHelicopter()
{
	// The width of the building mesh, used to align them to grid
	float BuildingWidth = CityBuilding->GetBoundingBox().GetSize().X;
	
	// We first initialise the ending location
	FActorSpawnParameters HeliSpawnParams;
	FVector EndSpawnLocation = this->GetActorLocation();

	// We then add the relative position from the index and column count
	EndSpawnLocation.X += (EndingPosition % Columns) * BuildingWidth;
	EndSpawnLocation.Y += FMath::FloorToInt(EndingPosition / Columns) * BuildingWidth;

	// We put the helicopter there and set the mesh
	AHelicopter* Helicopter = GetWorld()->SpawnActor<AHelicopter>(AHelicopter::StaticClass(), EndSpawnLocation, FRotator::ZeroRotator, HeliSpawnParams);
	Helicopter->SetActorScale3D(FVector(HelicopterScale));
	Helicopter->InitialiseMeshAndBounds(HelicopterMesh);
}