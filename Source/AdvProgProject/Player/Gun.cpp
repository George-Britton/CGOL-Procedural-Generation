// George Britton - Student# 100130736


#include "Gun.h"

#include "AdvProgProject/Enemies/Zombie.h"
#include "Components/AudioComponent.h"
#include "PlayerCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
UGun::UGun()
{
	// Here we add the particle system for the gunshot particles and sound
	GunshotParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Gunshot Particles"));
	GunshotParticleSystem->SetupAttachment(this);
	GunshotParticleSystem->bAutoManageAttachment = true;
	GunshotParticleSystem->bAutoActivate = false;
	GunshotSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Gunshot Sound"));
	GunshotSoundComponent->SetupAttachment(this);

	// We set the gun mesh to the component
	this->SetStaticMesh(GunMesh);
	this->SetCollisionProfileName("IgnoreAll");
	this->CastShadow = false;
}

// Called when a value changes
void UGun::CustomOnConstruction(UStaticMesh* InGunMesh, FTransform GunTransform, float GunDamage, float InFireRate, UParticleSystem* GunshotParticles, USoundBase* InGunshotSound, float InGunshotRange)
{
	SetRelativeTransform(GunTransform);
	Damage = GunDamage;
	GunshotRange = InGunshotRange;
	FireRate = InFireRate;
	GunshotParticleSystem->SetTemplate(GunshotParticles);
	GunMesh = InGunMesh;
	SetStaticMesh(GunMesh);
	if (GunMesh) if (GunMesh->GetName().Contains("M5")) GunshotParticleSystem->AutoAttachSocketName = "Barrel";
	GunshotSound = InGunshotSound;
	GunshotSoundComponent->SetSound(GunshotSound);
}

// Called every frame
void UGun::CustomTick(float DeltaTime)
{
	// Here we count down the fire timer...
	if (TimeSinceLastFire > 0.f) TimeSinceLastFire -= DeltaTime;
	// ... and if it's finished, and the player is firing, we call Fire()
	if (IsFiring && TimeSinceLastFire <= 0) Fire();
}

// FIRING
// Starts firing the player's gun
void UGun::ToggleFire(bool Firing)
{
	if (Firing) { IsFiring = true; }
	else IsFiring = false;
}
// Executes a fire from the gun
void UGun::Fire()
{
	FHitResult HitEnemy;
	FVector RayStart = PlayerCamera->GetComponentLocation();
	FVector RayEnd = RayStart + (PlayerCamera->GetForwardVector() * GunshotRange);
	FCollisionQueryParams CollisionParameters;

	DrawDebugLine(GetWorld(), RayStart, RayEnd, FColor::Red, false, 2, 0, 1);
	
	if (GetWorld()->LineTraceSingleByChannel(HitEnemy, RayStart, RayEnd, ECC_Visibility, CollisionParameters))
	{
		AZombie* ZombieTest = Cast<AZombie>(HitEnemy.GetActor());
		if (ZombieTest) ZombieTest->RecieveAttack(Damage);
	}
	
	// This makes the gun seem like it's firing, with the sound, particles, and announcing to the zombies that it's been fired
	GunshotSoundComponent->Play();
	GunshotParticleSystem->Activate(true);
	OnGunshot.Broadcast();
	TimeSinceLastFire = FireRate;
}