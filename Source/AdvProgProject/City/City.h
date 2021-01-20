// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityGenerator.h"
#include "City.generated.h"

struct FCellLifeRule;

UCLASS()
class ADVPROGPROJECT_API ACity : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACity();

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

	// GRID TECHNICALS
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

	// CITY
	// 'CityBuilding' is the static mesh that 'CityBuildingISMComponent' uses for populating the city
	UPROPERTY(EditAnywhere, Category = "City")
		UStaticMesh* CityBuilding = nullptr;
	UPROPERTY()
		UInstancedStaticMeshComponent* CityBuildingISMComponent = nullptr;
	// The rows and columns are used to set the size of the grid
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		int32 Rows = 50;
	UPROPERTY(EditAnywhere, Category = "City|Creation")
		int32 Columns = 50;

	// PROPS AND ROADS
	// These populate the city streets with random props and a road
	UPROPERTY(EditAnywhere, Category = "City|Props")
		TArray<UStaticMesh*> PropArray;
	UPROPERTY(EditAnywhere, Category = "City|Props")
		float PropSpawnProbability = 20.f;
	UPROPERTY()
		TArray<UInstancedStaticMeshComponent*> PropComponentArray;
	UPROPERTY(EditAnywhere, Category = "City|Road")
		UStaticMesh* RoadMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "City|Road")
		UMaterial* RoadMaterial = nullptr;
	UPROPERTY()
		UStaticMeshComponent* RoadComponent = nullptr;

	// ENDING
	// These variables are used to populate the ending tile of the grid
	UPROPERTY(EditAnywhere, Category = "City|Helicopter")
		USkeletalMesh* HelicopterMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "City|Helicopter")
		float HelicopterScale = 0.7f;

private:
	// INITIALISATION
	// Called to sanitize the input from the user
	void SanitizeInput();
	// Called to create the city
	void BeginCreateCity();

	// GENERATION
	// Called to create the city prop components and forest components
	void CreatePropComponents();
	// Finds a suitable start and end cell
	void FindEnds();
	// Initialises the population grid to be evolved
	void InitialiseGrid();
	// Called to create a randomly generated city
	void GenerateCity();
	// Checks that there is a walkable path for the player
	bool IsPathWalkable();
	TArray<int32> FloodFill(int32 Cell);

	// CELLULAR AUTOMATION
	// Counts the amounts of 'True' neighbours a grid cell has
	int32 CountLivingNeighbours(int32 Cell, TArray<int32> RelativeNeighbours);
	// Creates an array that stores the relative addresses of a cell's neighbours
	TArray<int32> CreateNeighbourArray();
	// Returns whether or not the cell provided is in the boarder
	bool IsWithinBorder(int32 Cell);
	// Return whether or not the cell is out of bounds
	bool IsOutOfBounds(int32 Cell);

	// CITY POPULATION
	// Called to populate the world
	bool BuildCity();
	void PlaceBuilding(FTransform PlacementTransform, float BuildingWidth);
	void PlaceProp(FTransform PlacementTransform, float BuildingWidth);
	void PlaceRoad();

	// Sets up the helicopter ending space
	void CreateHelicopter();

public:
	// Called to set the city parameters to the user-defined ones
	void ReceiveCityParameters(int32 InCityEvolutions, TArray<FCellLifeRule> InLifeRules, int32 InLifePercent, int32 InBorderWidth, UStaticMesh* InCityBuilding, int32 InRows, int32 InColumns, TArray<UStaticMesh*> InPropArray, float InPropProbability, UStaticMesh* InRoadMesh, UMaterial* InRoadMaterial, USkeletalMesh* InHeliMesh, float InHeliScale);

};
