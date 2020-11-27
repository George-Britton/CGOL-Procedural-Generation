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

	// This is the material that will be looping across our landscape
	UPROPERTY(EditAnywhere, Category = "Appearance")
		UMaterial* SeaMaterial = nullptr;
	// This is how far to the side the sea is spawned
	UPROPERTY(EditAnywhere, Category = "Appearance")
		float SeaDistance = 5000;
	// This is the container for the sea material and forms the base of the sea
	UPROPERTY()
		ALandscape* SeaLandscape = nullptr;

	// This is the component count of the landscape, i.e. the block size
	//UPROPERTY(EditAnywhere, Category = "Appearance")

	// Used to set the variables from external actors
	void ReceiveCreateSea(UMaterial* InSeaMaterial, float InSeaDistance);

private:
	// Creates the sea landscape using user-defined parameters
	void CreateSea();
};
