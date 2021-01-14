// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// global constants
const static float MAX_CAMERA_HEIGHT = 100.f;
const static float MAX_CAMERA_PITCH_LIMIT = 90.f;

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Here we create the camera component for the player and set it's parent and height
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	PlayerCamera->SetupAttachment(this->RootComponent);
	PlayerCamera->SetRelativeLocation(FVector(0, 0, CameraHeight));
	Gun = CreateDefaultSubobject<UGun>(TEXT("Gun"));
	Gun->SetupAttachment(PlayerCamera);

	// We make sure the player possesses the actor, and set the basic input settings
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
}

// Called when a value changes
void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	// Here we'll make sure the gun is in the right place for the player
	//if (Gun) Gun->CustomOnConstruction(GunTransform, GunshotParticleTransform);
	
	// Here we'll make sure all our values stay in a valid range
	CameraHeight = FMath::Clamp(CameraHeight, 1.f, MAX_CAMERA_HEIGHT);
	CameraPitchLimit = FMath::Clamp(CameraPitchLimit, 1.f, MAX_CAMERA_PITCH_LIMIT);
	RunSpeed = FMath::Clamp(RunSpeed, 1.f, RunSpeed);
	CrouchSpeed = FMath::Clamp(CrouchSpeed, 1.f, CrouchSpeed);
	BaseSpeed = FMath::Clamp(BaseSpeed, 1.f, BaseSpeed);
	JumpHeight = FMath::Clamp(JumpHeight, 1.f, JumpHeight);
	FireRate = FMath::Clamp(FireRate, 0.01f, FireRate);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Here we initialise the gun's appearance and rate of fire
	Gun->InitialiseGun(GunMesh, FireRate, GunshotParticles);

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

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Gun) Gun->CustomTick(DeltaTime);
	
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
	if (Running)
	{
		if (!IsRunning)
		{
			ToggleCrouch(false);
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			IsRunning = true;
		}
	}
	else
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
	else
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
	if (Jumping) { IsJumping = true; }
	else { IsJumping = false; }
}

// DEBUG
// This function is used to print errors that occur during runtime
void APlayerCharacter::PrintDebugMessage(FString DebugMessage)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, DebugMessage);
}