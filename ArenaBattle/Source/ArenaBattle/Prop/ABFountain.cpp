#include "Prop/ABFountain.h"
#include "Components/StaticMeshComponent.h"

AABFountain::AABFountain()
{
	PrimaryActorTick.bCanEverTick = false;

	body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));

	RootComponent = body;
	water->SetupAttachment(body);
	water->SetRelativeLocation( FVector{0.0f, 0.0f, 132.0f} );

	static ConstructorHelpers::FObjectFinder<UStaticMesh> bodyRef = TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> waterRef = TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02'");

	if (bodyRef.Object) body->SetStaticMesh(bodyRef.Object);
	if (waterRef.Object) water->SetStaticMesh(waterRef.Object);
}

void AABFountain::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

