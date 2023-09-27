#include "Character/ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringarmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABCharacterControlData.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_Jump.IA_Player_Jump'");
	if (InputActionJumpRef.Object) JumpAction = InputActionJumpRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ChangeActionRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_ChangeControl.IA_ChangeControl'");
	if (ChangeActionRef.Object) ChangeControlAction = ChangeActionRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderMoveRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_ShoulderMove.IA_Player_ShoulderMove'");
	if (ShoulderMoveRef.Object) ShoulderMoveAction = ShoulderMoveRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ShoulderLookRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_ShoulderLook.IA_Player_ShoulderLook'");
	if (ShoulderLookRef.Object) ShoulderLookAction = ShoulderLookRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> QuaterMoveRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Player_QuaterMove.IA_Player_QuaterMove'");
	if (QuaterMoveRef.Object) QuaterMoveAction = QuaterMoveRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> AttackRef = TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/IA_Attack.IA_Attack'");
	if (AttackRef.Object) AttackAction = AttackRef.Object;

	CurrentCharacterControlType = ECharacterControlType::Quater;
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Custom_Jump);
	enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AABCharacterPlayer::Custom_Jump);
	enhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
	enhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);
	enhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuaterMove);
	enhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharacterControl);
	enhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::CharacterAttack);
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterControl(CurrentCharacterControlType);
}

void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	SpringArm->TargetArmLength = CharacterControlData->TargetArmLength;
	SpringArm->SetRelativeRotation(CharacterControlData->RelativeRotation);
	SpringArm->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	SpringArm->bInheritPitch = CharacterControlData->bInheritPitch;
	SpringArm->bInheritYaw = CharacterControlData->bInheritYaw;
	SpringArm->bInheritRoll = CharacterControlData->bInheritRoll;
	SpringArm->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();

	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rot.Yaw, 0);

	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir, v.X);
	AddMovementInput(RightDir, v.Y);
}

// ��Ʈ�ѷ��� ���� ������� ȸ���� ������ �����ϴ� ����, ���̳� ī�޶� �����Ű�� �ʴ´�.
// �׷��� ����Ǵ� ������, ī�޶� springarm �� bUseControllerRotation �� üũ�Ǿ� �ֱ� ����
// ��, ���� bUseControllerRotation  �������� üũ�ϴ°� �ƴ϶�� MovementComponent�� ȸ��, �������� ó���ؾ� �Ѵ�.
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
	// x * x + y * y = z * z ���� x * x + y * y > 1.0f �� �������Ͱ� ������ �ƴϰ�, 1.0 < �̸� �������� �ϼ��� �ƴҼ���
	
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

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()); Subsystem) {
		Subsystem->ClearAllMappings();

		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;

		Subsystem->AddMappingContext(NewMappingContext, 0);
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

void AABCharacterPlayer::CharacterAttack(const FInputActionValue& value)
{
	bool v = value.Get<bool>();

	ProcessComboCommand();
}
