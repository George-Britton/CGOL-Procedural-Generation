// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieManager.h"
#include "AdvProgProject/Player/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

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
	}

	// Here we make sure the zombies register with the correct arrays
	InitialiseZombies();
}

// ACTIVATION SPHERES
// Used to add zombies to the appropriate arrays
void UZombieManager::OnSphereOverlap(AZombie* Zombie, UPrimitiveComponent* Sphere)
{
	if (Sphere->GetName() == "EnemyActivationSphere")
	{ ZombiesWithinActivationSphere.AddUnique(Zombie);}
	else if (Sphere->GetName() == "EnemyGunshotSphere")
	{ ZombiesWithinGunshotSphere.AddUnique(Zombie);	}
	else if (Sphere->GetName() == "EnemySightSphere")
	{
		ZombiesWithinChaseSphere.AddUnique(Zombie);
		Zombie->ToggleMoveToPlayer(true);
	}
}
// Used to remove zombies from the appropriate arrays
void UZombieManager::OnSphereEndOverlap(AZombie* Zombie, UPrimitiveComponent* Sphere)
{
	if (Sphere->GetName() == "EnemyActivationSphere")
	{
		ZombiesWithinActivationSphere.Remove(Zombie);
		Zombie->GetMesh()->SetRenderStatic(true);
	}
}
// Used to check all the zombies at the beginning to see what they should be involved in
void UZombieManager::InitialiseZombies()
{
	TArray<AActor*> Zombies;
	UGameplayStatics::GetAllActorsOfClass(this, AZombie::StaticClass(), Zombies);
	for (auto& Zombie : Zombies)
	{
		AZombie* TrueZombie = Cast<AZombie>(Zombie);
		TrueZombie->OnZombieDeath.AddDynamic(this, &UZombieManager::RemoveZombie);
		float DistanceToPlayer = UKismetMathLibrary::Vector_Distance(TrueZombie->GetActorLocation(), Player->GetActorLocation());
		AssessSpheres(DistanceToPlayer, TrueZombie);
	}
}
// Checks the spheres to see which ones the zombies should be in
bool UZombieManager::AssessSpheres(float Distance, AZombie* Zombie)
{
	if (Distance > Player->EnemyActivationSphereRadius) return false;
	ZombiesWithinActivationSphere.AddUnique(Zombie);
	if (Distance > Player->EnemyGunshotSphereRadius) return false;
	ZombiesWithinGunshotSphere.AddUnique(Zombie);
	if (Distance > Player->EnemySightSphereRadius) return false;
	ZombiesWithinChaseSphere.AddUnique(Zombie);
	return true;
}

// BEHAVIOUR
// Used to tell the zombie whether or not it should be rendering
void UZombieManager::CheckIfZombieShouldRender(AZombie* Zombie)
{
	FHitResult OutHit;
	FVector RayStart = Zombie->GetActorLocation();
	FVector RayEnd = Player->GetActorLocation();
	FCollisionQueryParams CollisionParameters;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, RayStart, RayEnd, ECC_Visibility, CollisionParameters))
	{
		if (OutHit.GetActor()->GetName().Contains("Player"))
		{
			Zombie->ToggleRender(true);
		}
		else Zombie->ToggleRender(false);
	}
}
// Used to tell zombies to chase gunfire
void UZombieManager::SendZombiesInRadiusToGunshot()
{
	for (auto& Zombie : ZombiesWithinGunshotSphere)
	{
		Zombie->ToggleMoveToPlayer(true);
	}
}
// Used to tell the zombie to chase the player on sight
void UZombieManager::ChasePlayerIfInLineOfSight(AZombie* Zombie)
{
	FHitResult OutHit;
	FVector RayStart = Zombie->GetActorLocation();
	FVector RayEnd = Player->GetActorLocation();
	FCollisionQueryParams CollisionParameters;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, RayStart, RayEnd, ECC_Visibility, CollisionParameters))
	{
		if (OutHit.GetActor()->GetName().Contains("Player"))
		{
			Zombie->ToggleMoveToPlayer(true);
		}
		else Zombie->ToggleMoveToPlayer(false);
	}
}

// LIFE
// Used to tell the manager to get rid of a dead zombie
void UZombieManager::RemoveZombie(AZombie* Zombie)
{
	ZombiesWithinActivationSphere.Remove(Zombie);
	ZombiesWithinGunshotSphere.Remove(Zombie);
	ZombiesWithinChaseSphere.Remove(Zombie);
}
