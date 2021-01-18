// Fill out your copyright notice in the Description page of Project Settings.


#include "APGameMode.h"

AAPGameMode::AAPGameMode()
{
	ZombieManager = CreateDefaultSubobject<UZombieManager>(TEXT("Zombie Manager"));
}