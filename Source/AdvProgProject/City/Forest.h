// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Forest.generated.h"

UCLASS()
class ADVPROGPROJECT_API AForest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AForest();

	// These populate the forest outside the city
	UPROPERTY(EditAnywhere, Category = "Appearance")
		TArray<UStaticMesh*> TreeArray;
	UPROPERTY(EditAnywhere, Category = "Appearance")
		uint32 ForestDensity = 25;
	UPROPERTY(EditAnywhere, Category = "Appearance")
		float ForestDistance = 50000.f;
	UPROPERTY(EditAnywhere, Category = "Appearance")
		float ForestLeniency = 2.f;
	UPROPERTY(EditAnywhere, Category = "Appearance")
		float ForestFalloff = 10.f;
	UPROPERTY()
		TArray<UInstancedStaticMeshComponent*> TreeComponentArray;

	// These are the variables from the city that are used to set the transform
	UPROPERTY()
		int32 Rows = 0;
	UPROPERTY()
		int32 Columns = 0;

	
	// Receives the command to create the forest
	void ReceiveCreateForest(TArray<UStaticMesh*> InTreeArray, uint32 InForestDensity, float InForestDistance, float InForestLeniency, float BuildingWidth, uint32 InRows, uint32 InColumns, float inForestFalloff);

private:
	
	// Called to populate the forest outside the city
	void PlantTrees(float BuildingWidth);
	// Checks that the tree is outside the city, but not too far
	bool ValidateTreeDistance(FVector TreeLocation, float BuildingWidth);

};
