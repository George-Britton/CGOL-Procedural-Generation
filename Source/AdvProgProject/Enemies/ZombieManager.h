// George Britton - Student# 100130736

#pragma once

#include "UObject/NoExportTypes.h"
#include "Zombie.h"
#include "ZombieSpawner.h"
#include "ZombieManager.generated.h"

// We must use multiple inheritence here to ensure that our base UObject is tickable
UCLASS()
class ADVPROGPROJECT_API UZombieManager : public UObject
{
	GENERATED_BODY()

public:
	// Standard constructor
	UZombieManager() {};

	// Reference to the player for raytracing
	UPROPERTY()
		class APlayerCharacter* Player = nullptr;

	// This array references which zombies are within the player's gunshot sphere
	UPROPERTY()
		TArray<AZombie*> ZombiesWithinGunshotSphere;

	// This array references the spawners that are within the player's spawn boundaries
	UPROPERTY()
		TArray<AZombieSpawner*> ZombieSpawnersInRange;

	// This controls how many zombies are alive at any given time
	UPROPERTY()
		int32 ZombiePopulation = 30;
	UPROPERTY()
		int32 ZombieSpawnStack = 0;
	
public:
	// INITIALISATION
	// This takes in the player as a world context reference and uses it to intialise the zombies
	void InitialiseActors(APlayerCharacter* InPlayer);
	
	// ACTIVATION SPHERES
	// Used to add zombies to the appropriate arrays
	UFUNCTION()
		void OnSphereOverlap(AActor* InActor, UPrimitiveComponent* Sphere);
	// Used to remove zombies from the appropriate arrays
	UFUNCTION()
		void OnSphereEndOverlap(AActor* InActor, UPrimitiveComponent* Sphere);
	// Used to check all the zombies at the beginning to see what they should be involved in
	void InitialiseSpawners();
	// Spawns a zombie
	void SpawnZombie();

	// BEHAVIOUR
	// Used to tell zombies to chase gunfire
	UFUNCTION()
		void SendZombiesInRadiusToGunshot();

	// LIFE
	// Used to tell the manager to get rid of a dead zombie
	UFUNCTION()
		void RemoveZombie(AZombie* Zombie);
};
