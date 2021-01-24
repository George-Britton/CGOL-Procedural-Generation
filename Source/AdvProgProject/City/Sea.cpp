// George Britton - Student# 100130736


#include "Sea.h"

#include "AdvProgProject/Player/PlayerCharacter.h"

// Sets default values
ASea::ASea()
{
	// Set this actor to not tick every frame.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SeaComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sea Component"));
	BeachComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beach Component"));
	SeaComponent->SetupAttachment(this->RootComponent);
	BeachComponent->SetupAttachment(this->RootComponent);
}

// Used to set the variables from external actors
void ASea::ReceiveCreateSea(UStaticMesh* InSeaMesh, UStaticMesh* InBeachMesh, float InSeaDistance, int32 InSeaHeight, int32 InSeaWidth, int32 InBeachWidth)
{
	// We first set the parameters from the function call
	SeaMesh = InSeaMesh;
	BeachMesh = InBeachMesh;
	SeaDistance = InSeaDistance;
	SeaHeight = InSeaHeight;
	SeaWidth = InSeaWidth;
	BeachWidth = InBeachWidth;

	// Then we initiate the components to own their meshes
	SeaComponent->SetStaticMesh(SeaMesh);
	BeachComponent->SetStaticMesh(BeachMesh);
	
	// And then we generate the sea
	CreateSea();
	Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()))->AcknowledgeSpawn();
}

// Creates the sea using user-defined parameters
void ASea::CreateSea()
{
	// First we move the sea to be running past the city with the user-defined distances
	// and calculating their relative distances from the city
	float TileWidth = SeaMesh->GetBoundingBox().GetSize().X;
	FVector BasePos = FVector(-SeaDistance, SeaWidth / 2 * TileWidth, 1.f);
	FVector BeachPos = FVector(-(BeachWidth / 2) * TileWidth, 0, 0);
	FVector SeaPos = FVector(-(SeaWidth / 2 + BeachWidth) * TileWidth, 0, 0);
	this->SetActorRelativeLocation(BasePos);
	BeachComponent->SetRelativeLocation(BeachPos);
	SeaComponent->SetRelativeLocation(SeaPos);

	// Then we set the meshes to span the length of the map
	SeaComponent->SetRelativeScale3D(FVector(SeaWidth, SeaHeight, 1.f));
	BeachComponent->SetRelativeScale3D(FVector(BeachWidth, SeaHeight, 1.f));
}