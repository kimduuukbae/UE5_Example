// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABCharacterControlData.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 700.0f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

    GetMesh()->SetRelativeLocationAndRotation(FVector{ 0.0f, 0.0f, -100.0f }, FRotator{ 0.0f, -90.0f, 0.0f });
    GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> characterMeshRef = TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'");
    if (characterMeshRef.Object) GetMesh()->SetSkeletalMesh(characterMeshRef.Object);

    static ConstructorHelpers::FClassFinder<UAnimInstance> animInstanceRef = TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C");
    if (animInstanceRef.Class) GetMesh()->SetAnimInstanceClass(animInstanceRef.Class);

    static ConstructorHelpers::FObjectFinder<UABCharacterControlData> shoulderDataRef = TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'");
    if (shoulderDataRef.Object) CharacterControlManager.Add(ECharacterControlType::Shoulder, shoulderDataRef.Object);

    static ConstructorHelpers::FObjectFinder<UABCharacterControlData> quaterDataRef = TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'");
    if (quaterDataRef.Object) CharacterControlManager.Add(ECharacterControlType::Quater, quaterDataRef.Object);
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
    // Pawn
    bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

    GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
    GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
    GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

