// George Britton - Student# 100130736

#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "CityGenerator.generated.h"

// Struct for the living rules of a cell, put into array for neighbour count
USTRUCT(BlueprintType)
struct FCellLifeRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		bool DeadCellBecomesLive;

	UPROPERTY(EditAnywhere)
		bool AliveCellStaysAlive;

	FCellLifeRule(bool Dead = false, bool Alive = false)
	{
		DeadCellBecomesLive = false;
		AliveCellStaysAlive = false;
	}
};

// This class is derived from the BoxComponent class, and is used to populate the cityscape
UCLASS()
class ADVPROGPROJECT_API ACityGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACityGenerator();

	// CELLULAR AUTOMATION
	// Number of times we evolve the cellular automation algorithm
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		int32 CityEvolutionGenerations = 6;
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		TArray<FCellLifeRule> CellLifeRules;
	// The percent chance our initial cell in PopGrid will die at the start
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		int32 CellLifePercent = 50;
	// This is how many layers of buildings should surround the entire play area
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		int32 BorderWidth = 2;

	// CITY
	// 'CityBuilding' is the static mesh that 'CityBuildingISMComponent' uses for populating the city
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		UStaticMesh* CityBuilding = nullptr;
	// The rows and columns are used to set the size of the grid
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		int32 Rows = 50;
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		int32 Columns = 50;
	int32 EndingPosition = 0;

	// PROPS AND ROADS
	// These populate the city streets with random props and a road
	UPROPERTY(EditAnywhere, Category = "City|Props")
		TArray<UStaticMesh*> PropArray;
	UPROPERTY(EditAnywhere, Category = "City|Props")
		float PropSpawnProbability = 20.f;
	UPROPERTY(EditAnywhere, Category = "City|Road")
		UStaticMesh* RoadMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "City|Road")
		UMaterial* RoadMaterial = nullptr;

	// FOREST
	// These populate the forest outside the city
	UPROPERTY(EditAnywhere, Category = "Forest")
		TArray<UStaticMesh*> TreeArray;
	UPROPERTY(EditAnywhere, Category = "Forest")
		uint32 ForestDensity = 25;
	UPROPERTY(EditAnywhere, Category = "Forest")
		float ForestDistance = 50000.f;
	UPROPERTY(EditAnywhere, Category = "Forest")
		float ForestLeniency = 2.f;
	UPROPERTY(EditAnywhere, Category = "Forest")
		float ForestFalloff = 10.f;

	// SEA
	// These set the conditions for the sea that spawns by the side of the city
	UPROPERTY(EditAnywhere, Category = "Sea")
		UStaticMesh* SeaMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Sea")
		UStaticMesh* BeachMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Sea")
		float SeaDistance = 5000;
	UPROPERTY(EditAnywhere, Category = "Sea")
		int32 SeaHeight = 2000;
	UPROPERTY(EditAnywhere, Category = "Sea")
		int32 SeaWidth = 1000;
	UPROPERTY(EditAnywhere, Category = "Sea")
		int32 BeachWidth = 300;

	// ENEMIES
	// These variables control how many zombies spawn and their variables
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int32 ZombiePopulation = 30;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int32 SpawnerFrequency = 50;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		USoundBase* ZombieRoarSound = nullptr;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		USkeletalMesh* ZombieMesh = nullptr;
	
	// ENDING
	// These variables are used to populate the ending tile of the grid
	UPROPERTY(EditAnywhere, Category = "City|Helicopter")
		USkeletalMesh* HelicopterMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "City|Helicopter")
		float HelicopterScale = 0.7f;

protected:
	
	// DEFAULT
	// Called whenever a value is changed
	void OnConstruction(const FTransform& Transform);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// CREATES EXTERNAL ACTORS
	// Constructs the city in the middle of the map
	void CreateCity();
	// Constructs the sea by the side of the city
	void CreateSea();
	// Sets up the forest outside the city
	void CreateForest();

	// Destroys itself after completion
	void DeleteSelf() { this->Destroy(); }
};