// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieController.h"
#include "AdvProgProject/Player/PlayerCharacter.h"
#include "Zombie.h"

const static float GLOBAL_ROAR_TIME = 2.f;

// Used to start the zombie moving towards the player
bool AZombieController::ToggleMoveToPlayer(APlayerCharacter* Player, bool Chasing, bool Idle, bool Roar)
{
	if (Chasing)
	{
		ParentZombie->ToggleRender(true);
		ParentZombie->ZombieState = EZombieState::CHASING;
		ParentZombie->OnZombieStateChange.Broadcast(EZombieState::CHASING);
		if (!Roar) { MoveToPlayer(); }
		else ParentZombie->Roar();
		return true;
	}
	else
	{
		StopMovement();
		ParentZombie->ZombieAttackCountdown = 0.f;
		if (Idle)
		{
			ParentZombie->ZombieState = EZombieState::IDLE;
			ParentZombie->OnZombieStateChange.Broadcast(EZombieState::IDLE);
		}
		return false;
	}
}
void AZombieController::MoveToPlayer() { MoveToActor(ParentZombie->Player, 50.f, false, true, true); }

// Used to stop the chase and attack the player
bool AZombieController::ToggleAttackPlayer(APlayerCharacter* Player, bool Attacking)
{
	if (Attacking)
	{
		ParentZombie->ZombieState = EZombieState::ATTACKING;
		ParentZombie->OnZombieStateChange.Broadcast(EZombieState::ATTACKING);
		return true;
	}
	else
	{
		ParentZombie->ToggleMoveToPlayer(true, false, false);
		return false;
	}
}