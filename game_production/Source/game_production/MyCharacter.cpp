#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"

// ===========================
// コンストラクタ
// ===========================
AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // ===== カメラ設定 =====
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.f;
    SpringArm->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // ===== キャラ回転設定 =====
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

    // ===== 空中制御設定 =====
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->JumpZVelocity = 420.f;

    // ===== 移動速度初期化 =====
    GetCharacterMovement()->MaxWalkSpeed = 300.f; // 歩き速度
}

// ===========================
// BeginPlay
// ===========================
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (IMC_Player)
                Subsystem->AddMappingContext(IMC_Player, 0);
        }
    }

}

// ===========================
// 入力設定
// ===========================
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (IA_Move)
            EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyCharacter::Move);

        if (IA_Look)
            EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

        if (IA_Jump)
        {
            EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyCharacter::StartJump);
            EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AMyCharacter::StopJump);
        }

        if (IA_Run)
        {
            EnhancedInput->BindAction(IA_Run, ETriggerEvent::Started, this, &AMyCharacter::StartRun);
            EnhancedInput->BindAction(IA_Run, ETriggerEvent::Completed, this, &AMyCharacter::StopRun);
        }

        // === 撮影用アクション（Enterキーなど） ===
        if (IA_TakePhoto)
        {
            EnhancedInput->BindAction(IA_TakePhoto, ETriggerEvent::Started, this, &AMyCharacter::StartTakePhoto);
        }
    }
}

// ===========================
// 移動処理
// ===========================
void AMyCharacter::Move(const FInputActionValue& Value)
{
    FVector2D Input = Value.Get<FVector2D>();
    if (Controller)
    {
        FRotator YawRot(0, Controller->GetControlRotation().Yaw, 0);
        FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

        AddMovementInput(Forward, Input.Y);
        AddMovementInput(Right, Input.X);
    }
}

// ===========================
// カメラ操作
// ===========================
void AMyCharacter::Look(const FInputActionValue& Value)
{
    FVector2D Axis = Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y * -1.0f);
}

// ===========================
// ジャンプ
// ===========================
void AMyCharacter::StartJump()
{
    bIsJumping = true;
    Jump();
}

void AMyCharacter::StopJump()
{
    bIsJumping = false;
    StopJumping();
}

// ===========================
// 走る
// ===========================
void AMyCharacter::StartRun()
{
    bIsRunning = true;
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AMyCharacter::StopRun()
{
    bIsRunning = false;
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

// ===========================
// 撮影アニメーション（Enterキー）
// ===========================
void AMyCharacter::StartTakePhoto()
{
    UE_LOG(LogTemp, Warning, TEXT("📸 StartTakePhoto called!"));

    if (bIsTakingPhoto) return; // 連打防止

    bIsTakingPhoto = true;
    UE_LOG(LogTemp, Warning, TEXT("📷 撮影開始！"));

    // 撮影中は移動を一時的に無効化
    GetCharacterMovement()->DisableMovement();

    // 1.0秒後に解除
    FTimerHandle Handle;
    GetWorldTimerManager().SetTimer(Handle, [this]()
        {
            bIsTakingPhoto = false;
            UE_LOG(LogTemp, Warning, TEXT("📷 撮影終了！"));
            GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        }, 1.0f, false);
}
