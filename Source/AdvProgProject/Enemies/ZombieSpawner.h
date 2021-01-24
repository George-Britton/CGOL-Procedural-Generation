// George Britton - Student# 100130736

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AdvProgProject/Config/ZombieAnimInstance.h"
#include "ZombieSpawner.generated.h"

UCLASS()
class ADVPROGPROJECT_API AZombieSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZombieSpawner();

	// Properties to give to the spawned zombies
	UPROPERTY(EditAnywhere, Category = "Zombie")
		USoundBase* RoarSound = nullptr;
	UPROPERTY(EditAnywhere, Category = "Zombie")
		UAnimBlueprint* AnimationBlueprint = nullptr;
	UPROPERTY(EditAnywhere, Category = "Zombie")
		USkeletalMesh* ZombieMesh = nullptr;

	// Function to spawn a zombie and return the reference
	AZombie* SpawnZombie();
};
