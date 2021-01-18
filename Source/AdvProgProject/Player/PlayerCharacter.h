// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Gun.h"
#include "AdvProgProject/Enemies/Zombie.h"
#include "PlayerCharacter.generated.h"

//class AZombie;

// We create a new delegate type the debug message function
DECLARE_DELEGATE_OneParam(FDebugDelegate, FString);
// We create delegate for the toggling of boolean states
DECLARE_DELEGATE_OneParam(FToggleState, bool);
// We create a delegate for the announcement of sphere overlap
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerSphereOverlap, class AZombie*, OtherActor, class UPrimitiveComponent*, OverlappedSphere);
// We also create a delegate for the announcement of sphere end overlaps
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerSphereEndOverlap, class AZombie*, OtherActor, class UPrimitiveComponent*, OverlappedSphere);
// We finally make a delegate for if the player dies
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);

UCLASS()
class ADVPROGPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Used to determine how much life the player has
	UPROPERTY(EditAnywhere, Category = "Life")
		float Health = 100.f;
	UPROPERTY(BlueprintAssignable, Category = "Life")
		FOnPlayerDeath OnPlayerDeath;

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
		float GunDamage = 20.f;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		float GunshotRange = 10000.f;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		UGun* Gun = nullptr;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		UStaticMesh* GunMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		float FireRate = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		FTransform GunTransform;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		UParticleSystem* GunshotParticles = nullptr;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		USoundBase* GunshotSound = nullptr;

	// Variables for the enemy activation spheres
	UPROPERTY(EditAnywhere, Category = "Enemies")
		float EnemyActivationSphereRadius = 10000.f;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		float EnemyGunshotSphereRadius = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		float EnemySightSphereRadius = 2500.f;
	UPROPERTY(VisibleAnywhere, Category = "Enemies")
		USphereComponent* EnemyActivationSphere = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Enemies")
		USphereComponent* EnemyGunshotSphere = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Enemies")
		USphereComponent* EnemySightSphere = nullptr;
	UPROPERTY(BlueprintAssignable, Category = "Enemies")
		FOnPlayerSphereOverlap OnPlayerSphereOverlap;
	UPROPERTY(BlueprintAssignable, Category = "Enemies")
		FOnPlayerSphereEndOverlap OnPlayerSphereEndOverlap;
	
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

	// COMPONENT SETUP
	// Creates the enemy interaction spheres
	void CreateEnemySpheres();
	// These three are used to announce when an enemy overlaps with a sphere
	UFUNCTION()
		void OnSphereOverlapFunction(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// These three are used to announce when an enemy ends overlap with a sphere
	UFUNCTION()
		void OnSphereEndOverlapFunction(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// Creates the camera and returns a reference for the gun
	UCameraComponent* CreateCamera();
	// Creates the gun using the CreateCamera() reference for the aiming
	void CreateGun(UCameraComponent* InCamera);
	
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

	// ATTACKS
	// Used to tell the player they have been attacked by a zombie
	void RecieveAttack(float Damage);

	// DEBUG
	// This function is used to print errors that occur during runtime
	UFUNCTION()
		void PrintDebugMessage(FString DebugMessage);
};
