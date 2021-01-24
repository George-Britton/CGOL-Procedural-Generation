// George Britton - Student# 100130736

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AdvProgProject/Enemies/Zombie.h"
#include "ZombieAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ADVPROGPROJECT_API UZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
		void InitParentZombie(AZombie* Zombie);
};
