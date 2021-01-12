// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gun.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVPROGPROJECT_API UGun : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGun();

	UPROPERTY(EditAnywhere, Category = "Firing")
		float FireRate = 0.5f;
	UPROPERTY()
		float TimeSinceLastFire = 0.f;
	UPROPERTY()
		bool IsFiring = false;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Starts firing the player's gun
	void StartFire();
	// Stops the player's gun from firing
	void StopFire();
};
