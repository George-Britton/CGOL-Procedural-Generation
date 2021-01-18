// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieController.h"
#include "AdvProgProject/Player/PlayerCharacter.h"
#include "Zombie.h"

// Used to start the zombie moving towards the player
bool AZombieController::ToggleMoveToPlayer(APlayerCharacter* Player, bool Chasing)
{
	if (Chasing)
	{
		StopMovement();
		ParentZombie->SetActorTickEnabled(true);
		MoveToActor(Player, 50.f, false, true, true);
		ParentZombie->ZombieState = EZombieState::CHASING;
		return true;
	}
	else
	{
		StopMovement();
		ParentZombie->ZombieState = EZombieState::IDLE;
		ParentZombie->ZombieAttackCountdown = 0.f;
		ParentZombie->SetActorTickEnabled(false);
		return false;
	}
}
// Used to stop the chase and attack the player
bool AZombieController::ToggleAttackPlayer(APlayerCharacter* Player, bool Attacking)
{
	if (Attacking)
	{
		ParentZombie->ToggleMoveToPlayer(false);
		ParentZombie->ZombieState = EZombieState::ATTACKING;
		return true;
	}
	else
	{
		ParentZombie->ToggleMoveToPlayer(true);
		return false;
	}
}