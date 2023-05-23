// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringarmComponent.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	
	springArm->SetupAttachment(RootComponent);
	springArm->TargetArmLength = 400.0f;
	springArm->bUsePawnControlRotation = true;

	camera->SetupAttachment(springArm, USpringArmComponent::SocketName);
	camera->bUsePawnControlRotation = false;


}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
}
