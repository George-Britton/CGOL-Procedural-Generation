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
	// Used to start the zombie moving towards the player
	void MoveToPlayer(AActor* Player);
};
