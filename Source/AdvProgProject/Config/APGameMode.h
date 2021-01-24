// George Britton - Student# 100130736

#pragma once

#include "AdvProgProject/Enemies/ZombieManager.h"
#include "AdvProgProject/Player/PlayerCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "APGameMode.generated.h"


UCLASS()
class ADVPROGPROJECT_API AAPGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// Standard constructor
	AAPGameMode();

	// Reference to the player for the loading and world management
	UPROPERTY()
		APlayerCharacter* Player = nullptr;

	// Controlled zombie manager
	UPROPERTY()
		UZombieManager* ZombieManager = nullptr;
	UPROPERTY()
		int32 ZombiePopulation = 30;

	UFUNCTION()
		void OnPlayerDeath() { UGameplayStatics::OpenLevel(Player, "DeathScreen"); }
	UFUNCTION()
		void OnPlayerReachesEnd() { UGameplayStatics::OpenLevel(Player, "WinScreen"); }

};
