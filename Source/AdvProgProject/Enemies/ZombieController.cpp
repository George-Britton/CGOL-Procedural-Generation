// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieController.h"
#include "AdvProgProject/Player/PlayerCharacter.h"
#include "Zombie.h"

// Used to start the zombie moving towards the player
bool AZombieController::ToggleMoveToPlayer(APlayerCharacter* Player, bool Chasing, bool Idle)
{
	if (Chasing)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, ParentZombie->GetName() + " chasing");
		StopMovement();
		ParentZombie->ToggleRender(true);
		MoveToActor(Player, 50.f, false, true, true);
		ParentZombie->ZombieState = EZombieState::CHASING;
		ParentZombie->OnZombieStateChange.Broadcast(EZombieState::CHASING);
		return true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, ParentZombie->GetName() + " stopped chasing");
		StopMovement();
		ParentZombie->ZombieAttackCountdown = 0.f;
		if (Idle)
		{
			ParentZombie->SetActorTickEnabled(false);
			ParentZombie->ZombieState = EZombieState::IDLE;
			ParentZombie->OnZombieStateChange.Broadcast(EZombieState::IDLE);
		}
		return false;
	}
}
// Used to stop the chase and attack the player
bool AZombieController::ToggleAttackPlayer(APlayerCharacter* Player, bool Attacking)
{
	if (Attacking)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, ParentZombie->GetName() + " attacking");
		ParentZombie->ZombieState = EZombieState::ATTACKING;
		ParentZombie->OnZombieStateChange.Broadcast(EZombieState::ATTACKING);
		return true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, ParentZombie->GetName() + " stopped attacking");
		ParentZombie->ToggleMoveToPlayer(true);
		return false;
	}
}