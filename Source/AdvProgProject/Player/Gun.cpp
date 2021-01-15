// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/AudioComponent.h"
#include "PlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"

// Sets default values
UGun::UGun()
{
	// Here we add the particle system for the gunshot particles and sound
	GunshotParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Gunshot Particles"));
	GunshotParticleSystem->SetupAttachment(this);
	GunshotParticleSystem->bAutoManageAttachment = true;
	GunshotParticleSystem->bAutoActivate = false;
	GunshotSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Gunshot Sound"));
	GunshotSoundComponent->SetupAttachment(this);

	// We set the gun mesh to the component
	this->SetStaticMesh(GunMesh);
	this->SetCollisionProfileName("IgnoreAll");
	this->CastShadow = false;
}

// Called when a value changes
void UGun::CustomOnConstruction(UStaticMesh* InGunMesh, FTransform GunTransform, float InFireRate, UParticleSystem* GunshotParticles, USoundBase* InGunshotSound, float InGunshotRange)
{
	SetRelativeTransform(GunTransform);
	GunshotRange = InGunshotRange;
	FireRate = InFireRate;
	GunshotParticleSystem->SetTemplate(GunshotParticles);
	GunMesh = InGunMesh;
	SetStaticMesh(GunMesh);
	if (GunMesh) if (GunMesh->GetName().Contains("M5")) GunshotParticleSystem->AutoAttachSocketName = "Barrel";
	GunshotSound = InGunshotSound;
	GunshotSoundComponent->SetSound(GunshotSound);
}

// Called every frame
void UGun::CustomTick(float DeltaTime)
{
	// Here we count down the fire timer...
	if (TimeSinceLastFire > 0.f) TimeSinceLastFire -= DeltaTime;
	// ... and if it's finished, and the player is firing, we call Fire()
	if (IsFiring && TimeSinceLastFire <= 0) Fire();
}

// FIRING
// Starts firing the player's gun
void UGun::ToggleFire(bool Firing)
{
	if (Firing) { IsFiring = true; }
	else IsFiring = false;
}
// Executes a fire from the gun
void UGun::Fire()
{
	FVector WorldLoc;
	FVector WorldDir;
	PlayerController->DeprojectScreenPositionToWorld(0.5f, 0.5f, WorldLoc, WorldDir);
		
	FHitResult HitEnemy;
	FVector RayStart = PlayerCamera->GetComponentLocation();
	FVector RayEnd = WorldLoc;
	FCollisionQueryParams CollisionParameters;

	DrawDebugLine(GetWorld(), RayStart, RayEnd, FColor::Red, false, 2, 0, 1);
	
	if (GetWorld()->LineTraceSingleByChannel(HitEnemy, RayStart, RayEnd, ECC_Visibility, CollisionParameters))
	{
		if (HitEnemy.bBlockingHit) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, "HIT: " + HitEnemy.GetActor()->GetName());
	}
	
	// TODO: Add Gun firing code
	GunshotSoundComponent->Play();
	GunshotParticleSystem->Activate(true);
	TimeSinceLastFire = FireRate;
}