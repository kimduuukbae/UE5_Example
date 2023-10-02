// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"
#include "Components/BoxComponent.h"
#include "Physics/ABCollision.h"
#include "Character/ABCharacterNonePlayer.h"
#include "Item/ABItemBox.h"

// Sets default values
AABStageGimmick::AABStageGimmick()
{
	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	RootComponent = Stage;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE'"));
	if (StageMeshRef.Object)
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
	}

	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTrigger->SetBoxExtent(FVector{ 775.0f, 775.0f, 300.0f });
	StageTrigger->SetupAttachment(Stage);
	StageTrigger->SetRelativeLocation(FVector{ 0.0f, 0.0f, 250.0f });
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnStageTriggerBeginOverlap);

	static FName GateSockets[] = { TEXT("+XGate"), TEXT("-XGate"), TEXT("+YGate"), TEXT("-YGate") };
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_GATE.SM_GATE'"));
	for (FName GateSocket : GateSockets)
	{
		UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GateSocket);
		Gate->SetStaticMesh(GateMeshRef.Object);
		Gate->SetupAttachment(Stage, GateSocket);
		Gate->SetRelativeLocation(FVector{ 0.0f, -80.0f, 0.0f });
		Gate->SetRelativeRotation(FRotator{ 0.0f, -90.0f, 0.0f });
		Gates.Add(GateSocket, Gate);

		FName TriggerName = *GateSocket.ToString().Append(TEXT("Trigger"));
		UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		GateTrigger->SetBoxExtent(FVector{ 100.0f, 100.0f, 300.0f });
		GateTrigger->SetupAttachment(Stage, GateSocket);
		GateTrigger->SetRelativeLocation(FVector{ 70.0f, 0.0f, 250.0f });
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnGateTriggerBeginOverlap);
		GateTrigger->ComponentTags.Add(GateSocket);

		GateTriggers.Add(GateTrigger);
	}

	CurrentState = EStageState::READY;
	StateChangeActions.Add(EStageState::READY, FStageChangedDelegateWrapper{ FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetReady) });
	StateChangeActions.Add(EStageState::FIGHT, FStageChangedDelegateWrapper{ FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetFight) });
	StateChangeActions.Add(EStageState::REWARD, FStageChangedDelegateWrapper{ FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseReward) });
	StateChangeActions.Add(EStageState::NEXT, FStageChangedDelegateWrapper{ FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseNext) });

	OpponentSpawnTime = 2.0f;
	OpponentClass = AABCharacterNonePlayer::StaticClass();

	RewardBoxClass = AABItemBox::StaticClass();

	for (FName GateSocket : GateSockets)
	{
		FVector BoxLocation = Stage->GetSocketLocation(GateSocket) / 2;
		RewardBoxLocations.Add(GateSocket, BoxLocation);
	}

	CurrentStageNum = 0;
}

void AABStageGimmick::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetState(CurrentState);
}

void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetState(EStageState::FIGHT);
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));

	FVector NewLocation = Stage->GetSocketLocation(SocketName);
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(GateTrigger), false, this);
	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams::InitType::AllStaticObjects,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParam
	);

#if ENABLE_DRAW_DEBUG
	FVector SphereOrigin = NewLocation;
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), SphereOrigin, 775.0f, 0, DrawColor, false, 5.0f);
#endif

	if (!bResult)
	{
		FTransform NewTransform{ NewLocation };
		AABStageGimmick* NewGimmick = GetWorld()->SpawnActorDeferred<AABStageGimmick>(AABStageGimmick::StaticClass(), NewTransform);
		
		NewGimmick->SetStageNum(CurrentStageNum + 1);
		NewGimmick->FinishSpawning(NewTransform);
	}
}

void AABStageGimmick::OpenAllGates()
{
	for (auto& Gate : Gates)
	{
		(Gate.Value)->SetRelativeRotation(FRotator{ 0.0f, -90.0f, 0.0f });
	}
}

void AABStageGimmick::CloseAllGate()
{
	for (auto& Gate : Gates)
	{
		(Gate.Value)->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void AABStageGimmick::SetState(EStageState InNewState)
{
	CurrentState = InNewState;

	if (StateChangeActions.Contains(InNewState))
		StateChangeActions[CurrentState].StageDelegate.ExecuteIfBound();
}

void AABStageGimmick::SetReady()
{
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	for (auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	OpenAllGates();
}

void AABStageGimmick::SetFight()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGate();

	GetWorld()->GetTimerManager().SetTimer(OpponentTimerHandle, this, &AABStageGimmick::OnOpponentSpawn, OpponentSpawnTime);
}

void AABStageGimmick::SetChooseReward()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGate();
	SpawnRewardBoxes();
}

void AABStageGimmick::SetChooseNext()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto& GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	}

	OpenAllGates();
}

void AABStageGimmick::OnOpponentDestroyed(AActor* DestroyedActor)
{
	SetState(EStageState::REWARD);
}

void AABStageGimmick::OnOpponentSpawn()
{
	const FTransform SpawnTransform{ GetActorLocation() + FVector::UpVector * 88.0f };
	AABCharacterNonePlayer* OpponentActor = GetWorld()->SpawnActorDeferred<AABCharacterNonePlayer>(OpponentClass, SpawnTransform);
	if (OpponentActor)
	{
		OpponentActor->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOpponentDestroyed);
		OpponentActor->SetLevel(CurrentStageNum);
		OpponentActor->FinishSpawning(SpawnTransform);
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for (const auto& RewardBox : RewardBoxes)
	{
		if (RewardBox.IsValid())
		{
			AABItemBox* ValidItemBox = RewardBox.Get();
			AActor* OverlappedBox = OverlappedComponent->GetOwner();
			if (OverlappedBox != ValidItemBox)
			{
				ValidItemBox->Destroy();
			}
		}
	}

	SetState(EStageState::NEXT);
}

void AABStageGimmick::SpawnRewardBoxes()
{
	for (const auto& RewardBoxLocation : RewardBoxLocations)
	{
		FVector WorldSpawnLocation = GetActorLocation() + RewardBoxLocation.Value + FVector{0.0f, 0.0f, 30.0f};
		FTransform NewTransform{ WorldSpawnLocation };
		
		AABItemBox* ItemActor = GetWorld()->SpawnActorDeferred<AABItemBox>(RewardBoxClass, NewTransform);

		if (ItemActor)
		{
			ItemActor->Tags.Add(RewardBoxLocation.Key);
			ItemActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnRewardTriggerBeginOverlap);
			RewardBoxes.Add(ItemActor);

			ItemActor->FinishSpawning(NewTransform);
		}
	}
}

