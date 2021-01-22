// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Zombie.h"
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

	// Used to tell if the manager is ready to start ray tracing to the player
	UPROPERTY()
		bool IsReady = false;
	UPROPERTY()
		float RayTraceTimer = 0.f;
	UPROPERTY()
		float RayTraceFrequency = 2.f;
	
public:
	// INITIALISATION
	// This takes in the player as a world context reference and uses it to intialise the zombies
	void InitialiseActors(APlayerCharacter* InPlayer);
	
	// ACTIVATION SPHERES
	// Used to add zombies to the appropriate arrays
	UFUNCTION()
		void OnSphereOverlap(AZombie* Zombie, UPrimitiveComponent* Sphere);
	// Used to remove zombies from the appropriate arrays
	UFUNCTION()
		void OnSphereEndOverlap(AZombie* Zombie, UPrimitiveComponent* Sphere);
	// Used to check all the zombies at the beginning to see what they should be involved in
	void InitialiseZombies(AZombie* InZombie);
	// Checks the spheres to see which ones the zombies should be in
	void AssessSpheres(float Distance, AZombie* Zombie);

	// BEHAVIOUR
	// Used to tell zombies to chase gunfire
	UFUNCTION()
		void SendZombiesInRadiusToGunshot();

	// LIFE
	// Used to tell the manager to get rid of a dead zombie
	UFUNCTION()
		void RemoveZombie(AZombie* Zombie);
};
