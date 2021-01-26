// George Britton - Student# 100130736

#pragma once

#include "Camera/CameraComponent.h"
#include "Engine.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Gun.generated.h"

// Delegate to alert the world that the gun has been fired
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGunshot);

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
		class APlayerCharacter* Player = nullptr;
	UPROPERTY()
		UCameraComponent* PlayerCamera = nullptr;

	// Firing variables
	UPROPERTY()
		float Damage = 20.f;
	UPROPERTY()
		float FireRate = 0.5f;
	UPROPERTY()
		float TimeSinceLastFire = 0.f;
	UPROPERTY()
		float GunshotRange = 10000.f;
	UPROPERTY()
		bool IsFiring = false;
	UPROPERTY()
		UParticleSystemComponent* GunshotParticleSystem = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Appearance")
		UParticleSystemComponent* BloodParticleSystem = nullptr;
	UPROPERTY()
		UAudioComponent* GunshotSoundComponent = nullptr;
	UPROPERTY()
		USoundBase* GunshotSound = nullptr;
	UPROPERTY()
		FOnGunshot OnGunshot;
	
	// Called when a value changes
	void CustomOnConstruction(UStaticMesh* InGunMesh, FTransform GunTransform, float GunDamage, float InFireRate, UParticleSystem* GunshotParticles, USoundBase* InGunshotSound, float InGunshotRange, UParticleSystem* BloodParticles);
	
	// Called every frame
	void CustomTick(float DeltaTime);

	// FIRING
	// Starts firing the player's gun
	void ToggleFire(bool Firing);
	// Executes a fire from the gun
	void Fire();
};
