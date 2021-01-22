// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieManager.h"
#include "AdvProgProject/Player/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "ZombieSpawner.h"


// INITIALISATION
// This takes in the player as a world context reference and uses it to intialise the zombies
void UZombieManager::InitialiseActors(APlayerCharacter* InPlayer)
{
	// This sets the player reference and binds the event dispatchers to the relevant functions
	Player = InPlayer;
	if (Player)
	{
		Player->OnPlayerSphereOverlap.AddDynamic(this, &UZombieManager::OnSphereOverlap);
		Player->OnPlayerSphereEndOverlap.AddDynamic(this, &UZombieManager::OnSphereEndOverlap);
		Player->Gun->OnGunshot.AddDynamic(this, &UZombieManager::SendZombiesInRadiusToGunshot);
	} else  GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Error: No player set in zombie manager");

	
	// Here we make sure the zombies register with the correct arrays
	InitialiseZombies(nullptr);
	IsReady = true;
}

// ACTIVATION SPHERES
// Used to add zombies to the appropriate arrays
void UZombieManager::OnSphereOverlap(AZombie* Zombie, UPrimitiveComponent* Sphere)
{
	if (Sphere->GetName().Contains("Activation")) { Zombie->ToggleRender(true); }
	else if (Sphere->GetName().Contains("Gunshot")) { ZombiesWithinGunshotSphere.AddUnique(Zombie); }
	else if (Sphere->GetName().Contains("Sight")) { Zombie->ToggleMoveToPlayer(true, false, true); }
	else if (Sphere->GetName().Contains("Attack")) { Zombie->ToggleAttackPlayer(true); }
}
// Used to remove zombies from the appropriate arrays
void UZombieManager::OnSphereEndOverlap(AZombie* Zombie, UPrimitiveComponent* Sphere)
{
	if (Sphere->GetName().Contains("Activation"))
	{
		Zombie->ToggleMoveToPlayer(false);
		Zombie->ToggleRender(false);
	} else if (Sphere->GetName().Contains("Gunshot")) { ZombiesWithinGunshotSphere.Remove(Zombie); }
	else if (Sphere->GetName().Contains("Attack")) { Zombie->ToggleAttackPlayer(false); }
}
// Used to check all the zombies at the beginning to see what they should be involved in
void UZombieManager::InitialiseZombies(AZombie* InZombie)
{
	if (InZombie)
	{
		float DistanceToPlayer = UKismetMathLibrary::Vector_Distance(InZombie->GetActorLocation(), Player->GetActorLocation());
		AssessSpheres(DistanceToPlayer, InZombie);
	}
	else
	{
		TArray<AActor*> Zombies;
		UGameplayStatics::GetAllActorsOfClass(this, AZombie::StaticClass(), Zombies);
		if (Zombies.Num())
		{
			for (auto& Zombie : Zombies)
			{
				AZombie* TrueZombie = Cast<AZombie>(Zombie);
				TrueZombie->OnZombieDeath.AddDynamic(this, &UZombieManager::RemoveZombie);
				float DistanceToPlayer = UKismetMathLibrary::Vector_Distance(TrueZombie->GetActorLocation(), Player->GetActorLocation());
				AssessSpheres(DistanceToPlayer, TrueZombie);
			}
		}
		InitialiseZombies(Cast<AZombieSpawner>(UGameplayStatics::GetActorOfClass(Zombies[0], AZombieSpawner::StaticClass()))->SpawnZombie());
	}
}
// Checks the zombie's gunshot distance
void UZombieManager::AssessSpheres(float Distance, AZombie* Zombie)
{
	if (Distance <= Player->EnemyGunshotSphereRadius) ZombiesWithinGunshotSphere.AddUnique(Zombie);
	if (Distance > Player->EnemyActivationSphereRadius) Zombie->ToggleRender(false);
}

// BEHAVIOUR
// Used to tell zombies to chase gunfire
void UZombieManager::SendZombiesInRadiusToGunshot()
{ for (auto& Zombie : ZombiesWithinGunshotSphere) { Zombie->ToggleMoveToPlayer(true); }}

// LIFE
// Used to tell the manager to get rid of a dead zombie
void UZombieManager::RemoveZombie(AZombie* Zombie) { ZombiesWithinGunshotSphere.Remove(Zombie); }
