// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AdvProgProject/Config/APGameMode.h"
#include "AdvProgProject/Enemies/ZombieManager.h"
#include "AdvProgProject/City/CityGenerator.h"

// global constants
const static float MAX_CAMERA_HEIGHT = 100.f;
const static float MAX_CAMERA_PITCH_LIMIT = 90.f;
const static FVector DEFAULT_REQUIRED_GUN_LOCATION = FVector(12.f, 10.f, -12.f);
const static FQuat DEFAULT_REQUIRED_GUN_ROTATION = FRotator(0.f, -90.f, 0.f).Quaternion();
const static float DEFAULT_REQUIRED_GUN_SCALE = 0.2f;

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// We first make the player camera
	UCameraComponent* CustomCamera = CreateCamera();
	// And then the gun using the returned reference
	CreateGun(CustomCamera);
	
	GunTransform.SetLocation(DEFAULT_REQUIRED_GUN_LOCATION);
	GunTransform.SetRotation(DEFAULT_REQUIRED_GUN_ROTATION);
	GunTransform.SetScale3D(FVector(DEFAULT_REQUIRED_GUN_SCALE));
	
	// We also need to create the sphere components that overlap the enemies
	CreateEnemySpheres();
	
	// We make sure the player possesses the actor, and set the basic input settings
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
}

// Called when a value changes
void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	// Here we'll make sure the gun is in the right place for the player
	Gun->CustomOnConstruction(GunMesh, GunTransform, GunDamage, FireRate, GunshotParticles, GunshotSound, GunshotRange);
	
	// Here we'll make sure all our values stay in a valid range
	CameraHeight = FMath::Clamp(CameraHeight, 1.f, MAX_CAMERA_HEIGHT);
	CameraPitchLimit = FMath::Clamp(CameraPitchLimit, 1.f, MAX_CAMERA_PITCH_LIMIT);
	RunSpeed = FMath::Clamp(RunSpeed, 1.f, RunSpeed);
	CrouchSpeed = FMath::Clamp(CrouchSpeed, 1.f, CrouchSpeed);
	BaseSpeed = FMath::Clamp(BaseSpeed, 1.f, BaseSpeed);
	JumpHeight = FMath::Clamp(JumpHeight, 1.f, JumpHeight);
	FireRate = FMath::Clamp(FireRate, 0.01f, FireRate);

	// Valid range for the spheres means that the size order between them stays the same
	EnemyActivationSphereRadius = FMath::Clamp(EnemyActivationSphereRadius, EnemyGunshotSphereRadius, EnemyActivationSphereRadius);
	EnemyGunshotSphereRadius = FMath::Clamp(EnemyGunshotSphereRadius, EnemySightSphereRadius, EnemyActivationSphereRadius);
	EnemySightSphereRadius = FMath::Clamp(EnemySightSphereRadius, EnemyAttackSphereRadius, EnemyGunshotSphereRadius);
	EnemyAttackSphereRadius = FMath::Clamp(EnemyAttackSphereRadius, 1.f, EnemySightSphereRadius);
	EnemyActivationSphere->SetSphereRadius(EnemyActivationSphereRadius, true);
	EnemyGunshotSphere->SetSphereRadius(EnemyGunshotSphereRadius, true);
	EnemySightSphere->SetSphereRadius(EnemySightSphereRadius, true);
	EnemyAttackSphere->SetSphereRadius(EnemyAttackSphereRadius, true);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Here we makes sure the player is alerted when the city is ready, so it can initialise the player with the zombies and gamemode
	Cast<ACityGenerator>(UGameplayStatics::GetActorOfClass(this, ACityGenerator::StaticClass()))->OnCityReady.AddDynamic(this, &APlayerCharacter::InitialisePlayer);

	// If the gun is destroyed due to no mesh, we call this
	if (!Gun)
	{
		// We unbind the fire action from the gun, as it no longer exists
		InputComponent->RemoveActionBinding("Fire", EInputEvent::IE_Pressed);
		InputComponent->RemoveActionBinding("Fire", EInputEvent::IE_Released);

		// We then bind the input action to the print debug function with a message saying that there is no gun
		InputComponent->BindAction<FDebugDelegate>("Fire", EInputEvent::IE_Pressed, this, &APlayerCharacter::PrintDebugMessage, FString("Error: No Gun - Possibly caused by no gun mesh set on player character"));
	}
}

// Called to intialised the player with the managing systems
void APlayerCharacter::InitialisePlayer()
{
	// We tell the zombie manager that this is the player
	AAPGameMode* AdvProgGameMode = nullptr;
	while (!AdvProgGameMode) {
		AdvProgGameMode = Cast<AAPGameMode>(UGameplayStatics::GetGameMode(this));
		if (AdvProgGameMode)
		{
			AdvProgGameMode->Player = this;
			AdvProgGameMode->ZombieManager->InitialiseActors(this);
		}
		else PrintDebugMessage("Error: gamemode not found by PlayerCharacter");
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// We call tick on the gun
	if (Gun) Gun->CustomTick(DeltaTime);

	// If the player is on the ground, and is pressing jump, they jump
	if (IsJumping && !GetCharacterMovement()->IsFalling())
	{
		ToggleCrouch(false);
		LaunchCharacter(FVector(0, 0, JumpHeight), false, true);
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Here we bind the player's inputs to the actions' code
	InputComponent->BindAxis("MoveX", this, &APlayerCharacter::MoveX);
	InputComponent->BindAxis("MoveY", this, &APlayerCharacter::MoveY);
	InputComponent->BindAxis("RotateX", this, &APlayerCharacter::RotateX);
	InputComponent->BindAxis("RotateY", this, &APlayerCharacter::RotateY);
	InputComponent->BindAction<FToggleState>("Run", EInputEvent::IE_Pressed, this, &APlayerCharacter::ToggleRun, true);
	InputComponent->BindAction<FToggleState>("Run", EInputEvent::IE_Released, this, &APlayerCharacter::ToggleRun, false);
	InputComponent->BindAction<FToggleState>("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacter::ToggleCrouch, true);
	InputComponent->BindAction<FToggleState>("Crouch", EInputEvent::IE_Released, this, &APlayerCharacter::ToggleCrouch, false);
	InputComponent->BindAction<FToggleState>("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::ToggleJump, true);
	InputComponent->BindAction<FToggleState>("Jump", EInputEvent::IE_Released, this, &APlayerCharacter::ToggleJump, false);
	InputComponent->BindAction<FToggleState>("Fire", EInputEvent::IE_Pressed, Gun, &UGun::ToggleFire, true);
	InputComponent->BindAction<FToggleState>("Fire", EInputEvent::IE_Released, Gun, &UGun::ToggleFire, false);
}

// COMPONENT SETUP
// Creates the enemy interaction spheres
void APlayerCharacter::CreateEnemySpheres()
{
	// Here we create the enemy spheres and parent them to the root of the player for locked movement
	EnemyActivationSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Enemy Activation Sphere"));
	EnemyGunshotSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Enemy Gunshot Sphere"));
	EnemySightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Enemy Sight Sphere"));
	EnemyAttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Enemy Attack Sphere"));
	EnemyActivationSphere->SetupAttachment(this->RootComponent);
	EnemyGunshotSphere->SetupAttachment(this->RootComponent);
	EnemySightSphere->SetupAttachment(this->RootComponent);
	EnemyAttackSphere->SetupAttachment(this->RootComponent);

	// This binds their overlap and end overlap to event dispatcher functions
	EnemyActivationSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnSphereOverlapFunction);
	EnemyActivationSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnSphereEndOverlapFunction);
	EnemyGunshotSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnSphereOverlapFunction);
	EnemyGunshotSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnSphereEndOverlapFunction);
	EnemySightSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnSphereOverlapFunction);
	EnemySightSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnSphereEndOverlapFunction);
	EnemyAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnSphereOverlapFunction);
	EnemyAttackSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnSphereEndOverlapFunction);
}
// These three are used to announce when an enemy overlaps with a sphere
void APlayerCharacter::OnSphereOverlapFunction(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AZombie* TestZombie = Cast<AZombie>(OtherActor);
	if (TestZombie) OnPlayerSphereOverlap.Broadcast(TestZombie, HitComp);
}
// These three are used to announce when an enemy ends overlap with a sphere
void APlayerCharacter::OnSphereEndOverlapFunction(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AZombie* TestZombie = Cast<AZombie>(OtherActor);
	if (TestZombie) OnPlayerSphereEndOverlap.Broadcast(TestZombie, HitComp);
}
// Creates the camera and returns a reference for the gun
UCameraComponent* APlayerCharacter::CreateCamera()
{
	// Here we create the camera component for the player and set it's parent
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	PlayerCamera->SetupAttachment(this->RootComponent);
	PlayerCamera->SetRelativeLocation(FVector(0, 0, CameraHeight));
	return PlayerCamera;
}
// Creates the gun using the CreateCamera() reference for the aiming
void APlayerCharacter::CreateGun(UCameraComponent* InCamera)
{
	// We also set the gun here to parent to the camera, so it turns with the layer
	Gun = CreateDefaultSubobject<UGun>(TEXT("Gun"));
	Gun->SetupAttachment(InCamera);
	Gun->PlayerCamera = InCamera;
	Gun->PlayerController = Cast<APlayerController>(GetController());
}

// INPUTS
// Makes the player move left or right
void APlayerCharacter::MoveX(float AxisValue) { if (AxisValue) AddMovementInput(GetActorRightVector(), AxisValue); }
// Makes the player move forward or backward
void APlayerCharacter::MoveY(float AxisValue) { if (AxisValue) AddMovementInput(GetActorForwardVector(), AxisValue); }
// Makes the player look left or right
void APlayerCharacter::RotateX(float AxisValue) { if (AxisValue) AddActorLocalRotation(FRotator(0, AxisValue, 0)); }
// Makes the player look up or down
void APlayerCharacter::RotateY(float AxisValue)
{
	if (AxisValue)
	{
		float CurrentRotation = PlayerCamera->GetRelativeRotation().Pitch + AxisValue;
		if (CurrentRotation < CameraPitchLimit && CurrentRotation > -CameraPitchLimit) PlayerCamera->AddLocalRotation(FRotator(AxisValue, 0, 0));
	}
}
// Makes the player run
void APlayerCharacter::ToggleRun(bool Running)
{
	// If the player is not running but should be, the player stops crouching and starts running
	if (Running)
	{
		if (!IsRunning)
		{
			ToggleCrouch(false);
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			IsRunning = true;
		}
	}
	else // otherwise if the player shouldn't be running but are, they stop running
	{
		if (IsRunning)
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
			IsRunning = false;
		}
	}

}
// Crouches the player down
void APlayerCharacter::ToggleCrouch(bool Crouching)
{
	// If the player should be crouching, they stop running and/or jumping, and start crouching
	if (Crouching)
	{
		if (!IsCrouching)
		{
			ToggleJump(false);
			ToggleRun(false);
			GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 2);
			GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
			IsCrouching = true;
		}
	}
	else // otherwise, if the player shouldn't be crouching but is, they stand up
	{
		if (IsCrouching)
		{
			GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 2);
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
			IsCrouching = false;
		}
	}
}
// Makes the player keep jumping up
void APlayerCharacter::ToggleJump(bool Jumping)
{
	// This simply toggles whether or not the player should be jumping
	if (Jumping) { IsJumping = true; }
	else { IsJumping = false; }
}

// ATTACKS
// Used to tell the player they have been attacked by a zombie
void APlayerCharacter::RecieveAttack(float Damage)
{
	Health -= Damage;
	if (Health <= 0.f) OnPlayerDeath.Broadcast();
}
// Used to tell the zombie they are overlapping with the player
void APlayerCharacter::OnZombieOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{ if (OtherActor->GetName().Contains("Zombie")) Cast<AZombie>(OtherActor)->ToggleAttackPlayer(true); }
void APlayerCharacter::OnZombieEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{ if (OtherActor->GetName().Contains("Zombie")) Cast<AZombie>(OtherActor)->ToggleAttackPlayer(false); }

// DEBUG
// This function is used to print errors that occur during runtime
void APlayerCharacter::PrintDebugMessage(FString DebugMessage)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, DebugMessage);
}