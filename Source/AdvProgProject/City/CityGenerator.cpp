// George Britton - Student# 100130736


#include "CityGenerator.h"

#include "AdvProgProject/Config/APGameMode.h"
#include "Forest.h"
#include "Helicopter.h"
#include "Sea.h"
#include "City.h"
#include "Kismet/GameplayStatics.h"

// DEFAULT
// Sets default values
ACityGenerator::ACityGenerator()
{
	// Set this actor to not tick every frame.
	PrimaryActorTick.bCanEverTick = false;

	// We now construct the components and attach them all to the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
}
// Called whenever a value changes
void ACityGenerator::OnConstruction(const FTransform& Transform)
{
	// We clamp the rows and columns so they can only be positive
	Rows = FMath::Clamp(Rows, 2, Rows);
	Columns = FMath::Clamp(Columns, 2, Columns);

	// We also clamp the prop spawn percent to between 0 and 100
	PropSpawnProbability = FMath::Clamp(PropSpawnProbability, 0.f, 100.f);

	// We finally clamp the border width to be between 0 and to have room to build a city
	BorderWidth = FMath::Clamp(BorderWidth, 0, Rows - 2);
	BorderWidth = FMath::Clamp(BorderWidth, 0, Columns - 2);
}
// Called when the game starts or when spawned
void ACityGenerator::BeginPlay()
{
	Super::BeginPlay();

	// We then generate the world, and declare it ready for the player
	Cast<AAPGameMode>(UGameplayStatics::GetGameMode(this))->ZombiePopulation = ZombiePopulation;
	CreateCity();
	CreateSea();
	CreateForest();
	DeleteSelf();
}

// CREATES EXTERNAL ACTORS
// Constructs the city in the middle of the map
void ACityGenerator::CreateCity()
{
	// We create a base spawn parameter for the city
	FActorSpawnParameters SpawnParams;

	// Then we spawn the city
	ACity* City = GetWorld()->SpawnActor<ACity>(ACity::StaticClass(), this->GetTransform(), SpawnParams);

	// Then we pass in all the user-define parameters
	City->ReceiveCityParameters(CityEvolutionGenerations, CellLifeRules, CellLifePercent, BorderWidth, CityBuilding, Rows, Columns, PropArray, PropSpawnProbability, RoadMesh, RoadMaterial, HelicopterMesh, HelicopterScale, SpawnerFrequency, ZombieRoarSound, ZombieMesh);
}
// Constructs the sea by the side of the city
void ACityGenerator::CreateSea()
{
	// We create a base spawn parameter for the sea
	FActorSpawnParameters SpawnParams;

	// Then we create the sea actor
	ASea* Sea = GetWorld()->SpawnActor<ASea>(ASea::StaticClass(), this->GetTransform(), SpawnParams);
	
	// Then we initialise the sea with its user-defined parameters
	Sea->ReceiveCreateSea(SeaMesh, BeachMesh, SeaDistance, SeaHeight, SeaWidth, BeachWidth);
}
// Sets up the forest outside the city
void ACityGenerator::CreateForest()
{
	// We use the building width here to allow the trees to determine the size of the city
	float BuildingWidth = CityBuilding->GetBoundingBox().GetSize().X;
	
	// We create a base spawn parameter for the forest
	FActorSpawnParameters SpawnParams;

	// Then we spawn the forest through the world manager
	AForest* Forest = GetWorld()->SpawnActor<AForest>(AForest::StaticClass(), this->GetTransform(), SpawnParams);

	// And finally we initialise it with the user-defined parameters
	Forest->ReceiveCreateForest(TreeArray, ForestDensity, ForestDistance, ForestLeniency, BuildingWidth, Rows, Columns, ForestFalloff);
}
