// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Gun.h"

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
	Gun->RegisterComponent();

	// We make sure the player possesses the actor, and set the basic input settings
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsJumping && !GetCharacterMovement()->IsFalling())
	{
		StopCrouch();
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
	InputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &APlayerCharacter::StartRun);
	InputComponent->BindAction("Run", EInputEvent::IE_Released, this, &APlayerCharacter::StopRun);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacter::StartCrouch);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &APlayerCharacter::StopCrouch);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::StartJump);
	InputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &APlayerCharacter::StopJump);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, Gun, &UGun::StartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, Gun, &UGun::StopFire);
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
void APlayerCharacter::StartRun()
{
	StopCrouch();
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	IsRunning = true;
}
// Slows the player down to their walking pace
void APlayerCharacter::StopRun()
{
	if (IsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		IsRunning = false;
	}
}
// Crouches the player down
void APlayerCharacter::StartCrouch()
{
	StopJumping();
	StopRun();
	GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 2);
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	IsCrouching = true;
}
// stands the player up
void APlayerCharacter::StopCrouch()
{
	if (IsCrouching)
	{
		GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 2);
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		IsCrouching = false;
	}
}
// Makes the player keep jumping up
void APlayerCharacter::StartJump() { IsJumping = true; }
// Stops the player from jumping up
void APlayerCharacter::StopJump() { IsJumping = false; }
