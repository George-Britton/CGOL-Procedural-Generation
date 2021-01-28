// George Britton - Student# 100130736

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieController.h"
#include "Zombie.generated.h"

// enum used to manage the zombies' finite state machine
UENUM(BlueprintType)
enum class EZombieState : uint8
{
	INACTIVE  UMETA(DisplayName = "Inactive"),
	IDLE UMETA(DisplayName = "Idle"),
	CHASING UMETA(DisplayName = "Chasing"),
	ATTACKING  UMETA(DisplayName = "Attacking"),
	DYING UMETA(DisplayName = "Dying"),
	MAX
};

// Delegate to tell the world that a zombie has died
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZombieDeath, AZombie*, Zombie);
// Here we have a delegate that announces a zombie state change
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZombieStateChange, EZombieState, State);

UCLASS()
class ADVPROGPROJECT_API AZombie : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombie();

	// Controls how much health the zombie has
	UPROPERTY()
		float Health = 50.f;
	UPROPERTY(BlueprintAssignable, Category = "Life")
		FOnZombieDeath OnZombieDeath;
	
	// Reference to the player for chasing and attacking
	UPROPERTY()
		class APlayerCharacter* Player = nullptr;
	UPROPERTY()
		float DespawnDistance = 10000.f;
	// Used to tell the player how much damage they're taking on an attack
	UPROPERTY()
		float Strength = 10.f;
	UPROPERTY(EditAnywhere, Category = "Attack")
		float BaseStrength = 10.f;
	UPROPERTY(EditAnywhere, Category = "Attack")
		float StrengthRandomisationPercent = 20.f;
	
	// Used to tell the zombie how to act
	UPROPERTY()
		AZombieController* ZombieController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
		EZombieState ZombieState = EZombieState::IDLE;
	UPROPERTY(BlueprintAssignable, Category = "States")
		FOnZombieStateChange OnZombieStateChange;

	// used to space out the zombie attack timings
	UPROPERTY()
		float ZombieAttackSpeed = 2.133333f;
	UPROPERTY()
		float ZombieAttackCountdown = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attack")
		USoundBase* RoarSound = nullptr;
	UPROPERTY()
		UAudioComponent* RoarSoundComponent = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// UTILITY	
	// Used to set the render of the zombie
	void ToggleRender(bool Rendering);
	// Used to set the zombie to chase down the player
	void ToggleMoveToPlayer(bool Chasing, bool Idle = false, bool Roar = true);
	void Roar();
	// Used to make the zombie attack the player
	void ToggleAttackPlayer(bool Attacking);
	void Attack();
	// Used to tell the zombie they've been shot
	void RecieveAttack(float InDamage);
	// Used to tell the zombie to delete itself, used by the animation
	UFUNCTION(BlueprintCallable, Category = "Life")
		void KillZombie(){ OnZombieDeath.Broadcast(this); this->Destroy(); }
};
