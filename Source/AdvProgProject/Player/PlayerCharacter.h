// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gun.h"
#include "PlayerCharacter.generated.h"

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
	UPROPERTY()
		UGun* Gun = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
	void StartRun();
	// Slows the player down to their walking pace
	void StopRun();
	// Crouches the player down
	void StartCrouch();
	// stands the player up
	void StopCrouch();
	// Makes the player jump up
	void StartJump();
	// Stops the player from jumping up
	void StopJump();
};
