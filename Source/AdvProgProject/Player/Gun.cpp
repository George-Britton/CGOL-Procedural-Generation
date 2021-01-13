// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

// Sets default values for this component's properties
UGun::UGun()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGun::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGun::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsFiring)
	{
		TimeSinceLastFire += DeltaTime;
		if (TimeSinceLastFire >= FireRate)
		{
			// TODO: Add Gun firing code
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "BANG!");
			TimeSinceLastFire = 0.f;
		}
	}
}

// Starts firing the player's gun
void UGun::StartFire() { IsFiring = true; }
// Stops the player's gun from firing
void UGun::StopFire() { IsFiring = false; TimeSinceLastFire = 0.f; }