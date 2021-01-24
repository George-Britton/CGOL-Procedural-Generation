// George Britton - Student# 100130736


#include "ZombieSpawner.h"
#include "AdvProgProject/Config/ZombieAnimInstance.h"
#include "AdvProgProject/Config/APGameMode.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ZombieManager.h"

// Sets default values
AZombieSpawner::AZombieSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
}

// Function to spawn a zombie and return the reference
void AZombieSpawner::SpawnZombie()
{
	// Set the spawn parameters
	FVector ZombieSpawnLocation = FVector(GetActorLocation().X + FMath::RandRange(-500.f, 500.f), GetActorLocation().Y + FMath::RandRange(-500.f, 500.f), GetActorLocation().Z);
	FRotator ZombieSpawnRotator = FRotator(0.f, FMath::RandRange(0.f, 359.f), 0.f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	// Spawn and initalise zombie
	AZombie* SpawnedZombie = GetWorld()->SpawnActor<AZombie>(ZombieSpawnLocation, ZombieSpawnRotator, SpawnParams);
	InitZombie(SpawnedZombie);
}

// Initialises the variables of the newly spawned zombie
void AZombieSpawner::InitZombie(AZombie* Zombie)
{
	// Set the roar
	Zombie->RoarSound = RoarSound;
	Zombie->RoarSoundComponent->SetSound(RoarSound);

	// Set the mesh and animation
	Zombie->GetMesh()->SetSkeletalMesh(ZombieMesh);
	Zombie->GetMesh()->SetAnimInstanceClass(AnimationBlueprint->GetAnimBlueprintGeneratedClass());
	Cast<UZombieAnimInstance>(Zombie->GetMesh()->GetAnimInstance())->InitParentZombie(Zombie);

	// Bind the zombie manager to the zombie's death delegate, and finally render
	Zombie->OnZombieDeath.AddDynamic(Cast<AAPGameMode>(UGameplayStatics::GetGameMode(this))->ZombieManager, &UZombieManager::RemoveZombie);
}