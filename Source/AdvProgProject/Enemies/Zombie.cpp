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
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->AddLocalOffset(FVector(0.f, 0.f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
	GetMesh()->AddLocalRotation(FRotator(0.f, -90.f, 0.f));
}

// Called when the game starts or when spawned
void AZombie::BeginPlay()
{
	Super::BeginPlay();
	
	// First we grab the reference to the zombie's controller
	ZombieController = Cast<AZombieController>(GetController());
	ZombieController->ParentZombie = this;

	// Here we randomise the zombie's strength within the user-define parameters
	Strength += FMath::RandRange(-(StrengthRandomisationPercent / BaseStrength), BaseStrength / StrengthRandomisationPercent);
	
	// Here we fins an actor of class "player character", and cast it to the right class for the player
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()));
}

// Called every frame
void AZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, GetName() + " state: " + FString::FromInt(int32(ZombieState)));
	
	if (ZombieAttackCountdown > 0) ZombieAttackCountdown -= DeltaTime;
	
	if (ZombieState == EZombieState::ATTACKING)
	{ if (ZombieAttackCountdown <= 0.f) Attack(); }
}

// UTILITY
// Used to set the render of the zombie
void AZombie::ToggleRender(bool Rendering)
{
	if (Rendering)
	{
		this->GetMesh()->SetRenderStatic(true);
		ZombieState = EZombieState::IDLE;
		OnZombieStateChange.Broadcast(EZombieState::IDLE);
	}
	else
	{
		this->GetMesh()->SetRenderStatic(false);
		ZombieState = EZombieState::INACTIVE;
		OnZombieStateChange.Broadcast(EZombieState::INACTIVE);
	}
}
// Used to set the zombie to chase down the player
void AZombie::ToggleMoveToPlayer(bool Chasing, bool Idle)
{
	if (Chasing) { ZombieController->ToggleMoveToPlayer(Player, true, Idle); }
	else ZombieController->ToggleMoveToPlayer(Player, false, Idle);
}
// Used to make the zombie attack the player
void AZombie::ToggleAttackPlayer(bool Attacking)
{
	if (Attacking) { ZombieController->ToggleAttackPlayer(Player, true); }
	else
	{
		ZombieController->ToggleAttackPlayer(Player, false);
		ToggleMoveToPlayer(true, false);
	}
}
void AZombie::Attack()
{
	Player->RecieveAttack(Strength);
	ZombieAttackCountdown = ZombieAttackSpeed;
}
// Used to tell the zombie they've been shot
void AZombie::RecieveAttack(float InDamage)
{
	Health -= InDamage;
	if (Health <= 0.f)
	{
		ZombieState = EZombieState::DYING;
		OnZombieStateChange.Broadcast(EZombieState::DYING);
		OnZombieDeath.Broadcast(this);
	}
}
