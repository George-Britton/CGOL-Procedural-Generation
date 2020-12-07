// Fill out your copyright notice in the Description page of Project Settings.


#include "Forest.h"

// Sets default values
AForest::AForest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Receives the command to create the forest
void AForest::ReceiveCreateForest(TArray<UStaticMesh*> InTreeArray, uint32 InForestDensity, float InForestDistance, float InForestLeniency, float BuildingWidth, uint32 InRows, uint32 InColumns, float inForestFalloff)
{
	// We se our forest variables to those passed in by the generator
	TreeArray = InTreeArray;
	ForestDensity = InForestDensity;
	ForestDistance = InForestDistance;
	ForestLeniency = InForestLeniency;
	Rows = InRows;
	Columns = InColumns;
	ForestFalloff = inForestFalloff;

	// Then we create the forest components
	if (TreeArray.Num())
	{
		// We initiate the array to be as big as the tree array
		TreeComponentArray.Init(nullptr, TreeArray.Num());

		// Then for each entry, we construct a component and add it to the root
		for (int32 TreeLooper = 0; TreeLooper < TreeArray.Num(); ++TreeLooper)
		{
			TreeComponentArray[TreeLooper] = Cast<UInstancedStaticMeshComponent>(StaticConstructObject_Internal(UInstancedStaticMeshComponent::StaticClass(), this));
			TreeComponentArray[TreeLooper]->RegisterComponent();
			TreeComponentArray[TreeLooper]->SetupAttachment(this->RootComponent);
			TreeComponentArray[TreeLooper]->SetStaticMesh(TreeArray[TreeLooper]);
			TreeComponentArray[TreeLooper]->SetWorldLocation(this->GetActorLocation());
		}
	}
	else GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Error: No trees in mesh array in " + this->GetName());
	// We add a debug string in case the tree array was empty but the forest was still created (hopefully will never happen)

	// Then we populate the forest
	PlantTrees(BuildingWidth);
}

// Called to populate the forest outside the city
void AForest::PlantTrees(float BuildingWidth)
{
	// Loops through the tree placing possibilities, with a max count of a density-controlled variable
	for (int32 TreeCounter = 0; TreeCounter < FMath::CeilToInt((Rows * Columns) * (ForestDensity / 100)); ++TreeCounter)
	{
		// We create a default vector to hold the spawn location
		FVector TreePlacement;

		// and we keep looping through a do while loop to make sure that the placement is valid
		do
		{
			TreePlacement.X = FMath::RandRange(-ForestDistance, (BuildingWidth * Columns) + ForestDistance);
			TreePlacement.Y = FMath::RandRange(-ForestDistance, (BuildingWidth * Rows) + ForestDistance);
		} while (!ValidateTreeDistance(TreePlacement, BuildingWidth));
		TreePlacement.Z = 0;

		// We assign the location to a transform, alongside a randomised rotation quaternion
		FTransform TreeTransform;
		TreeTransform.SetLocation(TreePlacement);
		FRotator TreeRotator = FRotator(0, FMath::RandRange(0, 359), 0);
		TreeTransform.SetRotation(TreeRotator.Quaternion());

		// and then we add the tree
		TreeComponentArray[FMath::RandRange(0, TreeComponentArray.Num() - 1)]->AddInstance(TreeTransform);
	}
}
// Checks that the tree is outside the city, but not too far
bool AForest::ValidateTreeDistance(FVector TreeLocation, float BuildingWidth)
{
	// If the tree is within the city, return false
	if (TreeLocation.X >= -0.25f * BuildingWidth && TreeLocation.X <= (Columns * BuildingWidth) + (BuildingWidth * 1.25f)
		&& TreeLocation.Y >= -0.25 * BuildingWidth && TreeLocation.Y <= (Rows * BuildingWidth) + (BuildingWidth * 1.25f))
		return false;

	if (ForestFalloff > 0)
		if (TreeLocation.X < -FMath::RandRange(0.f, ForestFalloff * BuildingWidth)) return false;
	if (ForestFalloff < 0)
		if (TreeLocation.X < -FMath::RandRange(ForestFalloff * BuildingWidth, 0.f)) return false;
	/*
	// We then loop through each tree component
	for (int32 CompLooper = 0; CompLooper < TreeComponentArray.Num(); ++CompLooper)
	{
		// if there are so far no trees of that type, we skip to the next component
		if (TreeComponentArray[CompLooper]->GetInstanceCount() == 0) continue;

		// We loop through each instance of this component's trees
		for (int32 InstanceLooper = 0; InstanceLooper < TreeComponentArray[CompLooper]->GetInstanceCount(); ++InstanceLooper)
		{
			FTransform TreeTransform;
			TreeComponentArray[CompLooper]->GetInstanceTransform(InstanceLooper, TreeTransform);
			// And if the new tree location is too close, we return false
			if (UKismetMathLibrary::Vector_Distance(TreeLocation, TreeTransform.GetLocation()) < TreeArray[CompLooper]->GetBoundingBox().GetSize().X / ForestLeniency) return false;
		}
	}*/
	// If none of these conditions are met, the tree placement is valid, and we place the tree
	return true;
}