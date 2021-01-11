// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieController.h"

// Used to start the zombie moving towards the player
void AZombieController::MoveToPlayer(AActor* Player)
{
	MoveToActor(Player, 50.f, true, true, true);
}