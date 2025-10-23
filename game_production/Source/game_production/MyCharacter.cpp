#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimInstance.h"

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // ========= カメラ設定 =========
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.f;
    SpringArm->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // ========= キャラ回転設定 =========
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

    // ========= 空中制御 =========
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->JumpZVelocity = 420.f;

    // ========= 移動速度初期値 =========
    GetCharacterMovement()->MaxWalkSpeed = 300.f; // 歩き速度
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Enhanced Input コンテキスト追加
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (IMC_Player)
                Subsystem->AddMappingContext(IMC_Player, 0);
        }
    }

    // ✅ ゲーム開始時にIdleアニメを再生
    if (IdleMontage)
    {
        GetMesh()->GetAnimInstance()->Montage_Play(IdleMontage);
        UE_LOG(LogTemp, Warning, TEXT("▶ 初期Idleを再生"));
    }
}


void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 地面に着地したらジャンプフラグを解除
    if (!GetCharacterMovement()->IsFalling() && bIsJumping)
    {
        bIsJumping = false;
        UpdateAnimation();
    }

    // 移動状態を毎フレーム速度から更新
    const float Speed = GetVelocity().Size2D();
    bool bWasMoving = bIsMoving;
    bIsMoving = Speed > 5.0f;

    // 状態が変化した時だけ更新
    if (bWasMoving != bIsMoving)
    {
        UpdateAnimation();
    }
}

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
    }
}

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

void AMyCharacter::Look(const FInputActionValue& Value)
{
    FVector2D Axis = Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y * -1.0f);
}

void AMyCharacter::StartJump()
{
    if (CanJump())
    {
        Jump();
        bIsJumping = true;
        UpdateAnimation();
    }
}

void AMyCharacter::StopJump()
{
    StopJumping();
}

void AMyCharacter::StartRun()
{
    bIsRunning = true;
    GetCharacterMovement()->MaxWalkSpeed = 600.f; // 走る速度
    UpdateAnimation();
}

void AMyCharacter::StopRun()
{
    bIsRunning = false;
    GetCharacterMovement()->MaxWalkSpeed = 300.f; // 歩く速度
    UpdateAnimation();
}

void AMyCharacter::UpdateAnimation()
{
    if (!GetMesh()) return;

    const float Speed = GetVelocity().Size2D();
    bIsMoving = Speed > 5.0f;

    UAnimMontage* AnimToPlay = nullptr;

    if (bIsJumping)
        AnimToPlay = JumpMontage;
    else if (bIsMoving)
        AnimToPlay = bIsRunning ? RunMontage : WalkMontage;
    else
        AnimToPlay = IdleMontage;

    UE_LOG(LogTemp, Warning, TEXT("UpdateAnimation: Running=%d, Moving=%d, Speed=%.1f"),
        bIsRunning, bIsMoving, Speed);

    PlaySmoothAnimation(AnimToPlay, 0.2f);
}

void AMyCharacter::PlaySmoothAnimation(UAnimMontage* MontageToPlay, float BlendTime)
{
    if (!MontageToPlay)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ MontageToPlay is NULL!"));
        return;
    }

    UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
    if (!AnimInst)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ AnimInstance is NULL!"));
        return;
    }

    // すでに同じモンタージュを再生中なら何もしない
    UAnimMontage* Current = AnimInst->GetCurrentActiveMontage();
    if (Current == MontageToPlay)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("▶ Playing Montage: %s"), *MontageToPlay->GetName());

    // スムーズに切り替える（違うモンタージュのときだけ停止）
    AnimInst->Montage_Stop(BlendTime);
    AnimInst->Montage_Play(MontageToPlay, 1.0f);
}
