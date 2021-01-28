// George Britton - Student# 100130736

#pragma once

#include "Components/BoxComponent.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Helicopter.generated.h"

class AHelicopter;
// The event dispatcher used to announce that the player has reached the end
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FOnPlayerReachEnd, AHelicopter, OnPlayerReachEnd);

UCLASS()
class ADVPROGPROJECT_API AHelicopter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHelicopter();

	// The appearance of the helicopter
	UPROPERTY(EditAnywhere, Category = "Appearance")
		USkeletalMeshComponent* HelicopterMesh = nullptr;
	UPROPERTY()
		USkeletalMeshComponent* HelicopterMeshComponent = nullptr;

	// The overlap box that ends the game
	UPROPERTY()
		UBoxComponent* EndOverlapBox = nullptr;
	UPROPERTY(EditAnywhere, Category = "End")
		float RelativeEndBoxSize = 1.5f;

	// Player's end-game event dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnPlayerReachEnd OnPlayerReachEnd;

	// Initialises the box around the added mesh
	void InitialiseMeshAndBounds(USkeletalMesh* Mesh);

};
