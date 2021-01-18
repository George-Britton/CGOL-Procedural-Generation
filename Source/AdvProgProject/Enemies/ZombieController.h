// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieController.generated.h"

/**
 * 
 */
UCLASS()
class ADVPROGPROJECT_API AZombieController : public AAIController
{
	GENERATED_BODY()

public:
	// Used to reference the parent of the controller
	UPROPERTY()
		class AZombie* ParentZombie;
	
	// Used to start the zombie moving towards the player
	bool ToggleMoveToPlayer(class APlayerCharacter* Player, bool Chasing);
	// Used to stop the chase and attack the player
	bool ToggleAttackPlayer(APlayerCharacter* Player, bool Attacking);
};
