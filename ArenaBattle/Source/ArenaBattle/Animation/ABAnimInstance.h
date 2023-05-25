// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	// ACharacter�� �ʱ�ȭ �� �� AnimInstance Ŭ������ �ν��Ͻ��� �����Ѵ�. 
	GENERATED_BODY()
	
public:
	UABAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;	// AnimInstance�� ó�� ������ �� �ѹ� ȣ���
	virtual void NativeUpdateAnimation(float dt) override;	// �����Ӹ��� ȣ���

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint32 bIsIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshould;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint32 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint32 bIsJumping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float JumpingThreshould;
};
