// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "ABComboActionData.h"
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    GetCapsuleComponent()->SetCollisionProfileName(CFROFILE_ABCAPSULE);

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 700.0f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

    GetMesh()->SetRelativeLocationAndRotation(FVector{ 0.0f, 0.0f, -100.0f }, FRotator{ 0.0f, -90.0f, 0.0f });
    GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> characterMeshRef = TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'");
    if (characterMeshRef.Object) GetMesh()->SetSkeletalMesh(characterMeshRef.Object);

    static ConstructorHelpers::FClassFinder<UAnimInstance> animInstanceRef = TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C");
    if (animInstanceRef.Class) GetMesh()->SetAnimInstanceClass(animInstanceRef.Class);

    static ConstructorHelpers::FObjectFinder<UABCharacterControlData> shoulderDataRef = TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'");
    if (shoulderDataRef.Object) CharacterControlManager.Add(ECharacterControlType::Shoulder, shoulderDataRef.Object);

    static ConstructorHelpers::FObjectFinder<UABCharacterControlData> quaterDataRef = TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'");
    if (quaterDataRef.Object) CharacterControlManager.Add(ECharacterControlType::Quater, quaterDataRef.Object);

    static ConstructorHelpers::FObjectFinder<UAnimMontage> comboActionMontageRef = TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack'");
    if (comboActionMontageRef.Object) ComboActionMontage = comboActionMontageRef.Object;

    static ConstructorHelpers::FObjectFinder<UABComboActionData> comboActionDataRef = TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_ComboAttack.ABA_ComboAttack'");
    if (comboActionDataRef.Object) ComboActionData = comboActionDataRef.Object;

    static ConstructorHelpers::FObjectFinder<UAnimMontage> deadMontageRef = TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'");
    if (deadMontageRef.Object) DeadMontage = deadMontageRef.Object;
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
    // Pawn
    bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

    GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
    GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
    GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void AABCharacterBase::ProcessComboCommand()
{
    if (CurrentCombo == 0)
    {
        ComboActionBegin();
        return;
    }


    if (!ComboTimerHandle.IsValid())
    {
        // 이미 타이머가 끝났거나, 더 이상 진행할 필요가 없다.
        HasNextComboCommand = false;
    }
    else 
    {
        HasNextComboCommand = true;
    }
}

void AABCharacterBase::ComboActionBegin()
{
    CurrentCombo = 1;

    // Movement Setting
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    // Animation Setting
    const float attackSpeedRate = 1.0f;

    UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
    animInstance->Montage_Play(ComboActionMontage, attackSpeedRate);

    FOnMontageEnded endDelegate;
    endDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);

    animInstance->Montage_SetEndDelegate(endDelegate, ComboActionMontage);

    ComboTimerHandle.Invalidate();
    SetComboCheckTimer();
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
    ensure(CurrentCombo != 0);
    CurrentCombo = 0;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AABCharacterBase::SetComboCheckTimer()
{
    int32 ComboIndex = CurrentCombo - 1;
    ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

    const float attackSpeedRate = 1.0f;
    float ComboEffectiveTime = ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate / attackSpeedRate;

    if (ComboEffectiveTime > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffectiveTime, false);
    }
}

void AABCharacterBase::ComboCheck()
{
    ComboTimerHandle.Invalidate();

    if (HasNextComboCommand) 
    {
        UAnimInstance* animInstance = GetMesh()->GetAnimInstance();

        CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);
        FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);

        animInstance->Montage_JumpToSection(NextSection, ComboActionMontage);

        SetComboCheckTimer();
        HasNextComboCommand = false;
    }
}

void AABCharacterBase::AttackHitCheck()
{
    FHitResult OutHitResult;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), true, this);

    const float AttackRange = 40.0f;
    const float AttackRadius = 50.0f;
    const float AttackDamage = 30.0f;
    const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
    const FVector End = Start + GetActorForwardVector() * AttackRange;

    bool HitDetected = GetWorld()->SweepSingleByChannel(
        OutHitResult,
        Start,
        End,
        FQuat::Identity,
        ECollisionChannel::ECC_GameTraceChannel1,
        FCollisionShape::MakeSphere(AttackRadius),
        Params
    );

    if (HitDetected)
    {
        // 무언가가 감지되었다는 뜻
        FDamageEvent DamageEvent;
        OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
    }

#if ENABLE_DRAW_DEBUG
    FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
    float CapsuleHalfHeight = AttackRange * 0.5f;
    FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

    DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
#endif
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    SetDead();

    return DamageAmount;
}

void AABCharacterBase::SetDead()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    PlayDeadAnimation();
    SetActorEnableCollision(false);
}

void AABCharacterBase::PlayDeadAnimation()
{
    UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
    animInstance->StopAllMontages(0.0f);

    animInstance->Montage_Play(DeadMontage, 1.0f);
}

