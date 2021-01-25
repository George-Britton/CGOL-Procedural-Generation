// George Britton - Student# 100130736


#include "ZombieManager.h"
#include "AdvProgProject/Config/APGameMode.h"
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
		
		// we also use the player as a world context object to get the gamemode's zombie population
		ZombiePopulation = Cast<AAPGameMode>(UGameplayStatics::GetGameMode(Player))->ZombiePopulation;
	} else  GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Error: No player set in zombie manager");

	
	// Here we make sure the zombies register with the correct arrays
	InitialiseSpawners();
}

// ACTIVATION SPHERES
// Used to add zombies to the appropriate arrays
void UZombieManager::OnSphereOverlap(AActor* InActor, UPrimitiveComponent* Sphere)
{
	// If the overlapped actor is a zombie, we check what behaviour to change depending on the sphere it overlapped
	if (!InActor->GetName().Contains("Spawner"))
	{
		AZombie* Zombie = Cast<AZombie>(InActor);
		if (Sphere->GetName().Contains("Activation")) { Zombie->ToggleRender(true); }
		else if (Sphere->GetName().Contains("Gunshot")) { ZombiesWithinGunshotSphere.AddUnique(Zombie); }
		else if (Sphere->GetName().Contains("Sight")) { Zombie->ToggleMoveToPlayer(true, false, true); }
		else if (Sphere->GetName().Contains("Attack")) { Zombie->ToggleAttackPlayer(true); }
	}else
	{ // if it's a spawner, we activate or deactivate it depending on distance
		AZombieSpawner* Spawner = Cast<AZombieSpawner>(InActor);
		if (Sphere->GetName().Contains("Spawn")) 
		{
			ZombieSpawnersInRange.AddUnique(Spawner);
			if (CurrentZombies < ZombiePopulation) SpawnZombie();
		}
		else if (Sphere->GetName().Contains("Activation")) { ZombieSpawnersInRange.Remove(Spawner); }
	}
}
// Used to remove zombies from the appropriate arrays
void UZombieManager::OnSphereEndOverlap(AActor* InActor, UPrimitiveComponent* Sphere)
{
	// If the overlapped actor is a zombie, we check what behaviour to change depending on the sphere it ended overlapping
	if (!InActor->GetName().Contains("Spawner"))
	{
		AZombie* Zombie = Cast<AZombie>(InActor);
		if (Sphere->GetName().Contains("Spawn")) { Zombie->KillZombie(); }
		else if (Sphere->GetName().Contains("Activation"))
		{
			Zombie->ToggleMoveToPlayer(false);
			Zombie->ToggleRender(false);
		}
		else if (Sphere->GetName().Contains("Gunshot")) { ZombiesWithinGunshotSphere.Remove(Zombie); }
		else if (Sphere->GetName().Contains("Attack")) { Zombie->ToggleAttackPlayer(false); }
	}else
	{ // if it's a spawner, we deactivate it
		AZombieSpawner* Spawner = Cast<AZombieSpawner>(InActor);
		if (Sphere->GetName().Contains("Spawn")) { ZombieSpawnersInRange.Remove(Spawner); }
	}
}
// Used to check all the zombies at the beginning to see what they should be involved in
void UZombieManager::InitialiseSpawners()
{
	TArray<AActor*> Spawners;
	UGameplayStatics::GetAllActorsOfClass(this, AZombieSpawner::StaticClass(), Spawners);

	// We loop through each zombie spawner, and if it's between the player's spawn boundaries, we add it to the active list
	if (Spawners.Num())
	{
		for (auto& Spawner : Spawners)
		{
			AZombieSpawner* TrueSpawner = Cast<AZombieSpawner>(Spawner);
			float DistanceToPlayer = UKismetMathLibrary::Vector_Distance(TrueSpawner->GetActorLocation(), Player->GetActorLocation());
			if (DistanceToPlayer <= Player->EnemySpawnSphereRadius && DistanceToPlayer > Player->EnemyActivationSphereRadius) ZombieSpawnersInRange.AddUnique(TrueSpawner);
		}

		// We then populate the world with the user-define amount of zombies
		SpawnZombie();
	}
}
// Spawns a zombie
void UZombieManager::SpawnZombie()
{
	// While there are any spawners within range and any zombies on the stack...
	while (ZombieSpawnersInRange.Num() > 0 && CurrentZombies < ZombiePopulation)
	{
		// We choose a spawner in range and spawn a zombie from it. We then reduce the stack by one.
		// We do it in this iterative way so if the spawners all leave the range, the rest of the stack remains until another spawns
		ZombieSpawnersInRange[FMath::RandRange(0, ZombieSpawnersInRange.Num() - 1)]->SpawnZombie();
		++CurrentZombies;
	}
}

// BEHAVIOUR
// Used to tell zombies to chase gunfire
void UZombieManager::SendZombiesInRadiusToGunshot()
{ for (auto& Zombie : ZombiesWithinGunshotSphere) { if (Zombie->ZombieState == EZombieState::IDLE) Zombie->ToggleMoveToPlayer(true); }}

// LIFE
// Used to tell the manager to get rid of a dead zombie
void UZombieManager::RemoveZombie(AZombie* Zombie)
{
	ZombiesWithinGunshotSphere.Remove(Zombie);
	Zombie->OnZombieDeath.Remove(this, "RemoveZombie");
	--CurrentZombies;
	SpawnZombie();
}
