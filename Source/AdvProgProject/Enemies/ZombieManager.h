// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Zombie.h"
#include "ZombieManager.generated.h"


UCLASS()
class ADVPROGPROJECT_API UZombieManager : public UObject
{
	GENERATED_BODY()

public:
	// Standard constructor
	UZombieManager();

	// Reference to the player for raytracing
	UPROPERTY()
		class APlayerCharacter* Player = nullptr;

	// These are arrays referencing which zombies are within the player's spheres
	UPROPERTY()
		TArray<AZombie*> ZombiesWithinActivationSphere;
	UPROPERTY()
		TArray<AZombie*> ZombiesWithinGunshotSphere;
	UPROPERTY()
		TArray<AZombie*> ZombiesWithinChaseSphere;

	// ACTIVATION SPHERES
	// Used to add zombies to the appropriate arrays
	UFUNCTION()
		void OnSphereOverlap(AZombie* Zombie, UPrimitiveComponent* Sphere);
	// Used to remove zombies from the appropriate arrays
	UFUNCTION()
		void OnSphereEndOverlap(AZombie* Zombie, UPrimitiveComponent* Sphere);
	// Used to check all the zombies at the beginning to see what they should be involved in
	void InitialiseZombies();
	// Checks the spheres to see which ones the zombies should be in
	bool AssessSpheres(float Distance, AZombie* Zombie);

	// BEHAVIOUR
	// Used to tell the zombie whether or not it should be rendering
	void CheckIfZombieShouldRender(AZombie* Zombie);
	// Used to tell zombies to chase gunfire
	UFUNCTION()
		void SendZombiesInRadiusToGunshot();
	// Used to tell the zombie to chase the player on sight
	void ChasePlayerIfInLineOfSight(AZombie* Zombie);

	// LIFE
	// Used to tell the manager to get rid of a dead zombie
	UFUNCTION()
		void RemoveZombie(AZombie* Zombie);
};
