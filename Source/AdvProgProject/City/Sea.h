// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Landscape.h"
#include "Sea.generated.h"

UCLASS()
class ADVPROGPROJECT_API ASea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASea();

	// This is the mesh that will be our sea
	UPROPERTY(EditAnywhere, Category = "Appearance")
		UStaticMesh* SeaMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Appearance")
		UStaticMesh* BeachMesh = nullptr;
	
	// This is how far to the side the sea is spawned
	UPROPERTY(EditAnywhere, Category = "Appearance")
		float SeaDistance = 5000;
	UPROPERTY(EditAnywhere, Category = "Appearance")
		int32 SeaHeight = 2000;
	UPROPERTY(EditAnywhere, Category = "Appearance")
		int32 SeaWidth = 1000;
	UPROPERTY(EditAnywhere, Category = "Appearance")
		int32 BeachWidth = 300;
	
	// These are the components that spawn the sea and beach meshes
	UPROPERTY()
		UStaticMeshComponent* SeaComponent;
	UPROPERTY()
		UStaticMeshComponent* BeachComponent;

	// Used to set the variables from external actors
	void ReceiveCreateSea(UStaticMesh* InSeaMesh, UStaticMesh* InBeachMesh, float InSeaDistance, int32 InSeaHeight, int32 InSeaWidth, int32 InBeachWidth);

private:
	// Creates the sea using user-defined parameters
	void CreateSea();
};
