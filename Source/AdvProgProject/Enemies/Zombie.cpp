// George Britton - Student# 100130736


#include "Zombie.h"
#include "AdvProgProject/Player/PlayerCharacter.h"

const static float MESH_ANIMATION_OFFSET = -112.f;

// Sets default values
AZombie::AZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Here we create the roar sound component and attach it to the root
	RoarSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Roar Component"));
	RoarSoundComponent->SetupAttachment(this->RootComponent);

	// Next we make sure the zombie has the right controller
	AIControllerClass = AZombieController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// And then we set the mesh customisations
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block); // For the ray trace from the gun
	GetMesh()->AddLocalOffset(FVector(0.f, 0.f, MESH_ANIMATION_OFFSET)); // To correct a mis-matched mesh
	GetMesh()->AddLocalRotation(FRotator(0.f, -90.f, 0.f)); // To correct a mis-matched mesh
}

// Called when the game starts or when spawned
void AZombie::BeginPlay()
{
	Super::BeginPlay();

	// We don't want zombies to be visible when first seen
	ToggleRender(false);

	// First we grab the reference to the zombie's controller
	ZombieController = Cast<AZombieController>(GetController());
	ZombieController->ParentZombie = this;
	while (!RoarSoundComponent->OnAudioFinished.IsBound()) { RoarSoundComponent->OnAudioFinished.AddDynamic(ZombieController, &AZombieController::MoveToPlayer); }

	// Here we randomise the zombie's strength within the user-define parameters
	Strength += FMath::RandRange(-(StrengthRandomisationPercent / BaseStrength), BaseStrength / StrengthRandomisationPercent);

	// We also need to add the roar sound to the sound component
	if (RoarSound) RoarSoundComponent->SetSound(RoarSound);
	
	// Here we fins an actor of class "player character", and cast it to the right class for the player
	while (!Player) { Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass())); }
	DespawnDistance = Player->EnemySpawnSphereRadius;
}

// Called every frame
void AZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// we reduce the attack countdown
	if (ZombieAttackCountdown > 0) ZombieAttackCountdown -= DeltaTime;

	// if the zombie is attacking, and the timer is at 0, attack the player
	if (ZombieState == EZombieState::ATTACKING)
	{
		if (ZombieAttackCountdown <= 0.f) Attack();
	}
}

// UTILITY
// Used to set the render of the zombie
void AZombie::ToggleRender(bool Rendering)
{
	if (Rendering)
	{
		this->GetMesh()->SetRenderStatic(false);
		this->GetMesh()->SetVisibility(true);
		ZombieState = EZombieState::IDLE;
		OnZombieStateChange.Broadcast(EZombieState::IDLE);
	}
	else
	{
		this->GetMesh()->SetRenderStatic(true);
		this->GetMesh()->SetVisibility(false);
		ZombieState = EZombieState::INACTIVE;
		OnZombieStateChange.Broadcast(EZombieState::INACTIVE);
	}
}
// Used to set the zombie to chase down the player
void AZombie::ToggleMoveToPlayer(bool Chasing, bool Idle, bool Roar)
{
	if (Chasing && ZombieState != EZombieState::DYING) { ZombieController->ToggleMoveToPlayer(Player, true, Idle, Roar); }
	else ZombieController->ToggleMoveToPlayer(Player, false, Idle, false);
}
void AZombie::Roar(){ RoarSoundComponent->Play(); }
// Used to make the zombie attack the player
void AZombie::ToggleAttackPlayer(bool Attacking)
{
	if (Attacking && ZombieState != EZombieState::DYING) { ZombieController->ToggleAttackPlayer(Player, true); }
	else
	{
		ZombieController->ToggleAttackPlayer(Player, false);
		ToggleMoveToPlayer(true, false, false);
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
	if (Health > 0.f)
	{
		Health -= InDamage;
		if (Health <= 0.f)
		{
			ZombieController->StopMovement();
			ZombieState = EZombieState::DYING;
			OnZombieStateChange.Broadcast(EZombieState::DYING);
		}
	}
}
