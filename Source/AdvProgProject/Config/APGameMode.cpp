// George Britton - Student# 100130736


#include "APGameMode.h"

// Standard constructor
AAPGameMode::AAPGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	ZombieManager = CreateDefaultSubobject<UZombieManager>(TEXT("Zombie Manager"));
}