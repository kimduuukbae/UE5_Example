// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringarmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	
	springArm->SetupAttachment(RootComponent);
	springArm->TargetArmLength = 400.0f;
	springArm->bUsePawnControlRotation = true;

	camera->SetupAttachment(springArm, USpringArmComponent::SocketName);
	camera->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> inputMappingRef = TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ArenaBattle/Input/IMC_Player.IMC_Player'");
	if (inputMappingRef.Object) defaultMappingContext = inputMappingRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> inputActionMoveRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_Move.IA_Player_Move'");
	if (inputActionMoveRef.Object) moveAction = inputActionMoveRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> inputActionLookRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_Look.IA_Player_Look'");
	if (inputActionLookRef.Object) lookAction = inputActionLookRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> inputActionJumpRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_Jump.IA_Player_Jump'");
	if (inputActionJumpRef.Object) jumpAction = inputActionJumpRef.Object;
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Custom_Jump);
	enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Completed, this, &AABCharacterPlayer::Custom_Jump);
	enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Move);
	enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Look);

}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* controller = CastChecked<APlayerController>(GetController());

	if (auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(controller->GetLocalPlayer()); subsystem) {
		subsystem->ClearAllMappings();

		subsystem->AddMappingContext(defaultMappingContext, 0);
	}
}

void AABCharacterPlayer::Move(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();

	const FRotator rot = Controller->GetControlRotation();
	const FRotator YawRotation(0, rot.Yaw, 0);

	const FVector forwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(forwardDir, v.X);
	AddMovementInput(rightDir, v.Y);
}

void AABCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();

	AddControllerYawInput(-v.X);
	AddControllerPitchInput(v.Y);
}

void AABCharacterPlayer::Custom_Jump(const FInputActionValue& value)
{
	bool v = value.Get<bool>();

	if (v)
		ACharacter::Jump();
	else
		ACharacter::StopJumping();
}
