// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "CityGenerator.generated.h"

class ACityGenerator;
// Event dispatcher for use after the city is generated and ready for the player
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FOnCityReady, ACityGenerator, OnCityReady);

// Enum to be used for the cardinal directions for the drunkard to step
UENUM(BlueprintType)
enum class EDirection : uint8
{
	NORTH UMETA(DisplayName = "North"),
	EAST UMETA(DisplayName = "East"),
	SOUTH UMETA(DisplayName = "South"),
	WEST UMETA(DisplayName = "West"),
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

	// 'CityBuilding' is a static mesh that 'CityISMComponent' uses for populating the city
	UPROPERTY(EditAnywhere, Category = "City")
		UStaticMesh* CityBuilding = nullptr;
	UPROPERTY()
		UInstancedStaticMeshComponent* CityISMComponent;

	// This box component is used to set the bounds of the city,
	// and the rows/columns are the size of the city grid
	UPROPERTY(EditAnywhere, Category = "City")
		UBoxComponent* CityBoundsBox;
	UPROPERTY()
		int32 Rows = 0;
	UPROPERTY()
		int32 Columns = 0;

	// The population grid and start/end positions of the city maze
	UPROPERTY()
		TArray<bool> PopulationGrid;
	UPROPERTY(EditAnywhere, Category = "City")
		int32 StartingPosition = 0;
	UPROPERTY(EditAnywhere, Category = "City")
		int32 EndingPosition = 0;

	// The event dispatcher for letting the player know it's ready to go
	UPROPERTY(BlueprintAssignable, Category = "City")
		FOnCityReady OnCityReady;

protected:
	// Called whenever a value is changed
	void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to implement the drunkard walk fractional brownian motion algorithm
	void DrunkardWalk();
	int32 Step(int32 DrunkardPosition);

	// Called to populate the world
	bool GenerateCity();
};
