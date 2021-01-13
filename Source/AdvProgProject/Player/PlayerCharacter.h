// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gun.h"
#include "PlayerCharacter.generated.h"

// We create a new delegate type the debug message function
DECLARE_DELEGATE_OneParam(FDebugDelegate, FString);
// and a delegate for the toggling of boolean states
DECLARE_DELEGATE_OneParam(FToggleState, bool);

UCLASS()
class ADVPROGPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Variables for the player's camera
	UPROPERTY()
		UCameraComponent* PlayerCamera = nullptr;
	UPROPERTY(EditAnywhere, Category = "Camera")
		float CameraHeight = 80.f;
	UPROPERTY(EditAnywhere, Category = "Camera")
		float CameraPitchLimit = 70.f;

	// Variables for the player's movement
	UPROPERTY(EditAnywhere, Category = "Movement")
		float RunSpeed = 1050.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float CrouchSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float BaseSpeed = 600.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float JumpHeight = 250.f;
	UPROPERTY()
		bool IsRunning = false;
	UPROPERTY()
		bool IsCrouching = false;
	UPROPERTY()
		bool IsJumping = false;

	// Variables for the player's weapon
	UPROPERTY(EditAnywhere, Category = "Weapon")
		UGun* Gun = nullptr;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		UStaticMesh* GunMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		float FireRate = 0.5f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when a value changes
	void OnConstruction(const FTransform& Transform) override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// INPUTS
	// Makes the player move left or right
	void MoveX(float AxisValue);
	// Makes the player move forward or backward
	void MoveY(float AxisValue);
	// Makes the player look left or right
	void RotateX(float AxisValue);
	// Makes the player look up or down
	void RotateY(float AxisValue);
	// Makes the player run
	void ToggleRun(bool Running);
	// Crouches the player down
	void ToggleCrouch(bool Crouching);
	// Makes the player jump up
	void ToggleJump(bool Jumping);

	// DEBUG
	// This function is used to print errors that occur during runtime
	void PrintDebugMessage(FString DebugMessage);
};
