// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Gun.generated.h"


UCLASS()
class ADVPROGPROJECT_API UGun : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	// Appearance variables
	UPROPERTY()
		UStaticMesh* GunMesh = nullptr;

	// Firing variables
	UPROPERTY()
		float FireRate = 0.5f;
	UPROPERTY()
		float TimeSinceLastFire = 0.f;
	UPROPERTY()
		bool IsFiring = false;
	UPROPERTY()
		UParticleSystemComponent* GunshotParticleSystem = nullptr;

	// Called to initialise the gun variables from the player-set parameters
	void InitialiseGun(UStaticMesh* InGunMesh, float InFireRate, UParticleSystem* GunshotParticles);

	// Called when a value changes
	void CustomOnConstruction(FTransform GunTransform, FTransform GunshotParticleTransform);

	// Called every frame
	void CustomTick(float DeltaTime);

	// FIRING
	// Starts firing the player's gun
	void ToggleFire(bool Firing);
	// Executes a fire from the gun
	void Fire();
};
