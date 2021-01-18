// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie.h"
#include "AdvProgProject/Player/PlayerCharacter.h"

// Sets default values
AZombie::AZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->GetMesh()->SetRenderStatic(true);
	AIControllerClass = AZombieController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	ZombieController = Cast<AZombieController>(GetController());
}

// Called when the game starts or when spawned
void AZombie::BeginPlay()
{
	Super::BeginPlay();

	// Here we randomise the zombie's strength within the user-define parameters
	Strength += FMath::RandRange(-(StrengthRandomisationPercent / BaseStrength), BaseStrength / StrengthRandomisationPercent);
	
	// Here we fins an actor of class "player character", and cast it to the right class for the player
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()));
}

// Called every frame
void AZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ZombieAttackCountdown > 0) ZombieAttackCountdown -= DeltaTime;
	
	if (ZombieState == EZombieState::ATTACKING)
	{
		if (ZombieAttackCountdown <= 0.f) Attack();
	}
}

// UTILITY
// Used to set the render of the zombie
void AZombie::ToggleRender(bool Rendering)
{
	if (Rendering) { this->GetMesh()->SetRenderStatic(true); }
	else this->GetMesh()->SetRenderStatic(false);
}
// Used to set the zombie to chase down the player
void AZombie::ToggleMoveToPlayer(bool Chasing)
{
	if (ZombieState != EZombieState::CHASING && Chasing)
	{ ZombieController->ToggleMoveToPlayer(Player, true); }
	else if (ZombieState == EZombieState::CHASING && !Chasing)
	{ ZombieController->ToggleMoveToPlayer(Player, false); }
}
// Used to make the zombie attack the player
void AZombie::ToggleAttackPlayer(bool Attacking)
{
	if (ZombieState != EZombieState::ATTACKING && Attacking)
	{ ZombieController->ToggleAttackPlayer(Player, true); }
	else if (ZombieState == EZombieState::ATTACKING && !Attacking)
	{ ZombieController->ToggleAttackPlayer(Player, false); }
}
void AZombie::Attack()
{
	Player->RecieveAttack(Strength);
}
// Used to tell the zombie they've been shot
void AZombie::RecieveAttack(float InDamage)
{
	Health -= InDamage;
	if (Health <= 0.f)
	{
		OnZombieDeath.Broadcast(this);
		this->Destroy();
	}
}
