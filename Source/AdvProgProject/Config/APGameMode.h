// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AdvProgProject/Enemies/ZombieManager.h"
#include "AdvProgProject/Player/PlayerCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "APGameMode.generated.h"


UCLASS()
class ADVPROGPROJECT_API AAPGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// Standard constructor
	AAPGameMode();

	// Reference to the player for the loading and world management
	UPROPERTY()
		APlayerCharacter* Player = nullptr;

	// Controlled zombie manager
	UPROPERTY()
		UZombieManager* ZombieManager = nullptr;

};
