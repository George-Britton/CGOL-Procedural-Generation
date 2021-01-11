// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie.h"
#include "ZombieController.h"

// Sets default values
AZombie::AZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->GetMesh()->SetRenderStatic(true);
	AIControllerClass = AZombieController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AZombie::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// UTILITY
// Used to set the render of the zombie
void AZombie::StopRender()
{
	this->GetMesh()->SetRenderStatic(true);
}
void AZombie::StartRender()
{
	this->GetMesh()->SetRenderStatic(false);
}
