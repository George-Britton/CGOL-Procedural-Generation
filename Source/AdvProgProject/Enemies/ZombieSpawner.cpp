// George Britton - Student# 100130736


#include "ZombieSpawner.h"
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
AZombie* AZombieSpawner::SpawnZombie()
{
	FActorSpawnParameters SpawnParams;
	AZombie* SpawnedZombie = GetWorld()->SpawnActor<AZombie>(this->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	SpawnedZombie->RoarSoundComponent->SetSound(RoarSound);
	SpawnedZombie->GetMesh()->SetSkeletalMesh(ZombieMesh);
	SpawnedZombie->GetMesh()->SetAnimClass(AnimationBlueprint);
	SpawnedZombie->ToggleRender(true);
	return SpawnedZombie;
}
