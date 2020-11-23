// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "CityGenerator.generated.h"
#include "Landscape.h"

class ACityGenerator;
// Event dispatcher for use after the city is generated and ready for the player
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FOnCityReady, ACityGenerator, OnCityReady);

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

// Enum to be used for the cardinal directions for the drunkard to step
UENUM(BlueprintType)
enum class EDifficulty : uint8
{
	EASY UMETA(DisplayName = "Easy"),
	NORMAL UMETA(DisplayName = "Normal"),
	HARD UMETA(DisplayName = "Hard"),
	MAX

};

// This class is derived from the BoxComponent class, and is used to populate the cityscape
UCLASS()
class ADVPROGPROJECT_API ACityGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACityGenerator();

	// The rows and columns are used to set the size of the grid
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		int32 Rows = 50;
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		int32 Columns = 50;

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
	
	// 'CityBuilding' is the static mesh that 'CityBuildingISMComponent' uses for populating the city
	UPROPERTY(EditAnywhere, Category = "City|Appearance|City|Buildings")
		UStaticMesh* CityBuilding = nullptr;
	UPROPERTY()
		UInstancedStaticMeshComponent* CityBuildingISMComponent = nullptr;

	// These populate the city streets with random props and a road
	UPROPERTY(EditAnywhere, Category = "City|Appearance|City|Props")
		TArray<UStaticMesh*> PropArray;
	UPROPERTY(EditAnywhere, Category = "City|Appearance|City|Props")
		float PropSpawnProbability = 20.f;
	UPROPERTY()
		TArray<UInstancedStaticMeshComponent*> PropComponentArray;
	UPROPERTY(EditAnywhere, Category = "City|Appearance|City|Road")
		UStaticMesh* RoadMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "City|Appearance|City|Road")
		UMaterial* RoadMaterial = nullptr;
	UPROPERTY()
		UInstancedStaticMeshComponent* RoadComponent = nullptr;

	// These populate the forest outside the city
	UPROPERTY(EditAnywhere, Category = "City|Appearance|Forest")
		TArray<UStaticMesh*> TreeArray;
	UPROPERTY(EditAnywhere, Category = "City|Appearance|Forest")
		uint32 ForestDensity = 25;
	UPROPERTY(EditAnywhere, Category = "City|Appearance|Forest")
		float ForestDistance = 50000.f;
	UPROPERTY(EditAnywhere, Category = "City|Appearance|Forest")
		float ForestLeniency = 2.f;
	UPROPERTY()
		TArray<UInstancedStaticMeshComponent*> TreeComponentArray;

	// This creates the sea
	UPROPERTY(EditAnywhere, Category = "City|Appearance|Sea")
		UMaterial* SeaMaterial = nullptr;
	UPROPERTY()
		ALandscape* SeaLandscape = nullptr;
	
	// These variables are used to populate the ending tile of the grid
	UPROPERTY(EditAnywhere, Category = "City|Appearance|City|Helicopter")
		USkeletalMesh* HelicopterMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "City|Appearance|City|Helicopter")
		float HelicopterScale = 0.7f;
	
	// The population grid and start/end positions of the city maze
	UPROPERTY()
		TArray<bool> PopulationGrid;
	UPROPERTY()
		int32 StartingPosition = 0;
	UPROPERTY()
		int32 EndingPosition = 0;

	// A grid used to flood fill in order to check for a walkable path
	UPROPERTY()
		TArray<bool> FloodArray;

	// The event dispatcher for letting the player know it's ready to go
	UPROPERTY(BlueprintAssignable, Category = "City")
		FOnCityReady OnCityReady;

protected:
	// Called whenever a value is changed
	void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to create the city prop components and forest components
	void CreatePropComponents();
	void CreateForestComponents();
	
	// Finds a suitable start and end cell
	void FindEnds();

	// Initialises the population grid to be evolved
	void InitialiseGrid();
	
	// Called to create a randomly generated city
	void GenerateCity();

	// Counts the amounts of 'True' neighbours a grid cell has
	int32 CountLivingNeighbours(int32 Cell, TArray<int32> RelativeNeighbours);

	// Creates an array that stores the relative addresses of a cell's neighbours
	TArray<int32> CreateNeighbourArray();
	
	// Returns whether or not the cell provided is in the boarder
	bool IsWithinBorder(int32 Cell);
	// Return whether or not the cell is out of bounds
	bool IsOutOfBounds(int32 Cell);

	// Checks that there is a walkable path for the player
	bool IsPathWalkable();
	TArray<int32> FloodFill(int32 Cell);
	
	// Called to populate the world
	bool BuildCity();
	void PlaceBuilding(FTransform PlacementTransform, float BuildingWidth);
	void PlaceProp(FTransform PlacementTransform, float BuildingWidth);
	void PlaceRoad(FTransform PlacementTransform, float BuildingWidth);

	// Called to populate the forest outside the city
	void PlantTrees(float BuildingWidth);
	// Checks that the tree is outside the city, but not too far
	bool ValidateTreeDistance(FVector TreeLocation, float BuildingWidth);

	// Called to create the sea landscape
	void CreateSea();
	
	// Sets up the helicopter ending space
	void CreateEnding(float BuildingWidth);
};