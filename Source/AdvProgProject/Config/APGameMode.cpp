// Fill out your copyright notice in the Description page of Project Settings.


#include "APGameMode.h"

// Standard constructor
AAPGameMode::AAPGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	ZombieManager = CreateDefaultSubobject<UZombieManager>(TEXT("Zombie Manager"));
}