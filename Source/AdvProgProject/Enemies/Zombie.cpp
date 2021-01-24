// George Britton - Student# 100130736


#include "Zombie.h"
#include "AdvProgProject/Player/PlayerCharacter.h"

const static float MESH_ANIMATION_OFFSET = -112.f;

// Sets default values
AZombie::AZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RoarSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Roar Component"));
	RoarSoundComponent->SetupAttachment(this->RootComponent);
	AIControllerClass = AZombieController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->AddLocalOffset(FVector(0.f, 0.f, MESH_ANIMATION_OFFSET));
	GetMesh()->AddLocalRotation(FRotator(0.f, -90.f, 0.f));
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
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()));
}

// Called every frame
void AZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// we reduce the attack countdown
	if (ZombieAttackCountdown > 0) ZombieAttackCountdown -= DeltaTime;

	// if the zombie is attacking, and the timer is at 0, attack the player
	if (ZombieState == EZombieState::ATTACKING)
	{ if (ZombieAttackCountdown <= 0.f) Attack(); }
}

// UTILITY
// Used to set the render of the zombie
void AZombie::ToggleRender(bool Rendering)
{
	if (Rendering)
	{
		this->GetMesh()->SetRenderStatic(false);
		this->GetMesh()->SetVisibility(true);
		SetActorTickEnabled(true);
		ZombieState = EZombieState::IDLE;
		OnZombieStateChange.Broadcast(EZombieState::IDLE);
	}
	else
	{
		this->GetMesh()->SetRenderStatic(true);
		this->GetMesh()->SetVisibility(false);
		SetActorTickEnabled(false);
		ZombieState = EZombieState::INACTIVE;
		OnZombieStateChange.Broadcast(EZombieState::INACTIVE);
	}
}
// Used to set the zombie to chase down the player
void AZombie::ToggleMoveToPlayer(bool Chasing, bool Idle, bool Roar)
{
	if (Chasing) { ZombieController->ToggleMoveToPlayer(Player, true, Idle, Roar); }
	else ZombieController->ToggleMoveToPlayer(Player, false, Idle, false);
}
void AZombie::Roar() { RoarSoundComponent->Play(); }
// Used to make the zombie attack the player
void AZombie::ToggleAttackPlayer(bool Attacking)
{
	if (Attacking) { ZombieController->ToggleAttackPlayer(Player, true); }
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
	Health -= InDamage;
	if (Health <= 0.f)
	{
		ZombieController->StopMovement();
		ZombieState = EZombieState::DYING;
		OnZombieStateChange.Broadcast(EZombieState::DYING);
		OnZombieDeath.Broadcast(this);
	}
}
