// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "PlayerCharacter.h"

// Called to initialise the gun variables from the player-set parameters
void UGun::InitialiseGun(UStaticMesh* InGunMesh, float InFireRate)
{
	// If the player passed in a mesh, set the local mesh, if none is passed in, destroy gun
	if (InGunMesh) { GunMesh = InGunMesh; }
	else
	{
		Cast<APlayerCharacter>(this->GetOwner())->Gun = nullptr;
		this->DestroyComponent();
	}

	// We set the gun mesh to the component
	this->SetStaticMesh(GunMesh);

	// We set the fire rate to be the user-set one
	if (InFireRate) FireRate = InFireRate;
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
	// TODO: Add Gun firing code
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "BANG!");
	TimeSinceLastFire = FireRate;
}