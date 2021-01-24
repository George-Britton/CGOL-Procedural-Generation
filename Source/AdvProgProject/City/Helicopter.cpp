// George Britton - Student# 100130736


#include "Helicopter.h"

#include "AdvProgProject/Config/APGameMode.h"


// Sets default values
AHelicopter::AHelicopter()
{
	// Set this actor to not tick every frame.
	PrimaryActorTick.bCanEverTick = false;

	// Here we create the component variables
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	HelicopterMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helicopter Mesh Component"));
	EndOverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("End Overlap Box"));
	HelicopterMeshComponent->SetupAttachment(this->RootComponent);
	EndOverlapBox->SetupAttachment(this->RootComponent);
	HelicopterMeshComponent->SetCollisionProfileName("BlockAll");
}

// Called when another actor overlaps with this actor
void AHelicopter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor->GetName().Contains("Player")) OnPlayerReachEnd.Broadcast();
}

// Initialises the box around the added mesh
void AHelicopter::InitialiseMeshAndBounds(USkeletalMesh* Mesh)
{
	HelicopterMeshComponent->SetSkeletalMesh(Mesh);
	EndOverlapBox->SetBoxExtent(Mesh->GetBounds().BoxExtent * RelativeEndBoxSize);
	this->SetActorRotation(FRotator(0, 45, 0));
	AAPGameMode* AdvProgGameMode = Cast<AAPGameMode>(UGameplayStatics::GetGameMode(this));
	OnPlayerReachEnd.AddDynamic(AdvProgGameMode, &AAPGameMode::OnPlayerReachesEnd);
}
