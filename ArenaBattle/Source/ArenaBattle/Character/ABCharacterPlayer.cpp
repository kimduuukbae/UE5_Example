// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringarmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABCharacterControlData.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	
	springArm->SetupAttachment(RootComponent);
	springArm->TargetArmLength = 400.0f;
	springArm->bUsePawnControlRotation = true;

	camera->SetupAttachment(springArm, USpringArmComponent::SocketName);
	camera->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<UInputAction> inputActionJumpRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_Jump.IA_Player_Jump'");
	if (inputActionJumpRef.Object) jumpAction = inputActionJumpRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ChangeActionRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_ChangeControl.IA_ChangeControl'");
	if (ChangeActionRef.Object) ChangeControlAction = ChangeActionRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderMoveRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_ShoulderMove.IA_Player_ShoulderMove'");
	if (ShoulderMoveRef.Object) ShoulderMoveAction = ShoulderMoveRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderLookRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_ShoulderLook.IA_Player_ShoulderLook'");
	if (ShoulderLookRef.Object) ShoulderLookAction = ShoulderLookRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> QuaterMoveRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_QuaterMove.IA_Player_QuaterMove'");
	if (QuaterMoveRef.Object) QuaterMoveAction = QuaterMoveRef.Object;

	CurrentCharacterControlType = ECharacterControlType::Quater;
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Custom_Jump);
	enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Completed, this, &AABCharacterPlayer::Custom_Jump);
	enhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
	enhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);
	enhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuaterMove);
	enhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharacterControl);
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterControl(CurrentCharacterControlType);
}

void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	springArm->TargetArmLength = CharacterControlData->TargetArmLength;
	springArm->SetRelativeRotation(CharacterControlData->RelativeRotation);
	springArm->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	springArm->bInheritPitch = CharacterControlData->bInheritPitch;
	springArm->bInheritYaw = CharacterControlData->bInheritYaw;
	springArm->bInheritRoll = CharacterControlData->bInheritRoll;
	springArm->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();

	const FRotator rot = Controller->GetControlRotation();
	const FRotator YawRotation(0, rot.Yaw, 0);

	const FVector forwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(forwardDir, v.X);
	AddMovementInput(rightDir, v.Y);
}

// 컨트롤러는 현재 사용자의 회전양 같은걸 관리하는 거지, 폰이나 카메라에 적용시키지 않는다.
// 그런데 적용되는 이유는, 카메라나 springarm 에 bUseControllerRotation 이 체크되어 있기 때문
// 즉, 폰도 bUseControllerRotation  같은것을 체크하는게 아니라면 MovementComponent로 회전, 움직임을 처리해야 한다.
void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();

	AddControllerYawInput(-v.X);
	AddControllerPitchInput(v.Y);
}

void AABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();
	// x * x + y * y = z * z 에서 x * x + y * y > 1.0f 면 단위벡터가 무조건 아니고, 1.0 < 이면 단위벡터 일수도 아닐수도
	
	if (MovementVectorSizeSquared > 1.0f) {
		MovementVectorSizeSquared = 1.0f;
		MovementVector.Normalize();
	}

	FVector MoveDirection = FVector{ MovementVector.X, MovementVector.Y, 0.0f };
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSizeSquared);
}

void AABCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::Quater)
		SetCharacterControl(ECharacterControlType::Shoulder);
	else if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
		SetCharacterControl(ECharacterControlType::Quater);
}

void AABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* controller = CastChecked<APlayerController>(GetController());

	if (auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(controller->GetLocalPlayer()); subsystem) {
		subsystem->ClearAllMappings();

		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;

		subsystem->AddMappingContext(NewMappingContext, 0);
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AABCharacterPlayer::Custom_Jump(const FInputActionValue& value)
{
	bool v = value.Get<bool>();

	if (v)
		ACharacter::Jump();
	else
		ACharacter::StopJumping();
}
