// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "Engine.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Gun.generated.h"


UCLASS()
class ADVPROGPROJECT_API UGun : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UGun();
	
	// Appearance variables
	UPROPERTY()
		UStaticMesh* GunMesh = nullptr;

	// variables about the player
	UPROPERTY()
		class APlayerController* PlayerController = nullptr;
	UPROPERTY()
		UCameraComponent* PlayerCamera = nullptr;

	// Firing variables
	UPROPERTY()
		float FireRate = 0.5f;
	UPROPERTY()
		float TimeSinceLastFire = 0.f;
	UPROPERTY()
		float GunshotRange = 10000.f;
	UPROPERTY()
		bool IsFiring = false;
	UPROPERTY(VisibleAnywhere, Category = "Appearance")
		UParticleSystemComponent* GunshotParticleSystem = nullptr;
	UPROPERTY()
		UAudioComponent* GunshotSoundComponent = nullptr;
	UPROPERTY()
		USoundBase* GunshotSound = nullptr;
	
	// Called when a value changes
	void CustomOnConstruction(UStaticMesh* InGunMesh, FTransform GunTransform, float InFireRate, UParticleSystem* GunshotParticles, USoundBase* InGunshotSound, float InGunshotRange);

	// Called every frame
	void CustomTick(float DeltaTime);

	// FIRING
	// Starts firing the player's gun
	void ToggleFire(bool Firing);
	// Executes a fire from the gun
	void Fire();
};
