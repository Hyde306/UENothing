#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AMyCharacter::AMyCharacter()
{
    // Tick関数を有効にする（毎フレーム処理を実行できるようにする）
    PrimaryActorTick.bCanEverTick = true;

    // ======== カメラの設定 ========
    // カメラをキャラクターの後ろに追従させるスプリングアームを作成
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);              // キャラのルートに接続
    SpringArm->TargetArmLength = 300.0f;                    // キャラとカメラの距離
    SpringArm->bUsePawnControlRotation = true;              // コントローラの回転を反映（マウス視点操作に追従）

    // カメラ本体を作成してスプリングアームに取り付け
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // スプリングアームの先端に配置
    FollowCamera->bUsePawnControlRotation = false;          // カメラ自体は独自に回転しない（スプリングアームが回転を担当）

    // ======== キャラクター回転の設定 ========
    // コントローラの回転にキャラクター本体が直接追従しないようにする
    bUseControllerRotationYaw = false;

    // 移動方向にキャラクターが自然に向きを変える設定
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // 回転スピード（Y軸のみ）

    // ======== 空中制御（エアコントロール）設定 ========
    // 空中でもある程度方向転換できるようにする
    GetCharacterMovement()->AirControl = 0.35f;                       // 空中方向転換のしやすさ
    GetCharacterMovement()->AirControlBoostMultiplier = 1.8f;         // 急な方向転換を補助
    GetCharacterMovement()->AirControlBoostVelocityThreshold = 25.f;  // ブーストが有効になる速度閾値
    GetCharacterMovement()->BrakingDecelerationFlying = 100.f;        // 空中で減速する速さ
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // ======== Enhanced Input のマッピング設定 ========
    // プレイヤーコントローラを取得
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        // Enhanced Input Subsystemを取得（入力を扱うシステム）
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            // プレイヤー用の入力マッピングコンテキストを追加
            if (IMC_Player)
            {
                Subsystem->AddMappingContext(IMC_Player, 0); // 優先度0で登録
            }
        }
    }
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced Input コンポーネントとしてキャスト
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // ======== 移動入力 ========
        if (IA_Move)
        {
            // WASDや左スティックなどの入力でMove関数を呼ぶ
            EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
        }

        // ======== ジャンプ入力 ========
        if (IA_Jump)
        {
            // ボタンを押した瞬間だけジャンプ（長押し無効）
            EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyCharacter::StartJump);

            // 下のコメントアウトを外すと「長押しジャンプ」が可能になる
            //EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &AMyCharacter::StartJump);
            //EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AMyCharacter::StopJump);
        }

        // ======== カメラ操作（マウス・右スティック） ========
        if (IA_Look)
        {
            EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
        }
    }
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
    // 入力値を取得（X:横, Y:前後）
    FVector2D Input = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // 現在のカメラ向きを取得（Yaw成分のみ使用）
        FRotator Rotation = Controller->GetControlRotation();
        FRotator YawRotation(0, Rotation.Yaw, 0);

        // 前方向と右方向のベクトルを算出
        FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // 入力方向に応じて移動
        AddMovementInput(ForwardDir, Input.Y);
        AddMovementInput(RightDir, Input.X);
    }
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
    // 視点操作入力値（X:左右, Y:上下）
    FVector2D LookAxis = Value.Get<FVector2D>();

    if (Controller)
    {
        // 水平回転（Yaw）
        AddControllerYawInput(LookAxis.X);

        // 垂直回転（Pitch）
        // *-1でY軸反転（マウスの上下操作を直感的にしたい場合は調整可能）
        AddControllerPitchInput(LookAxis.Y * -1.0f);
    }
}

void AMyCharacter::StartJump()
{
    // ジャンプ可能状態ならジャンプ開始
    if (CanJump())
    {
        Jump();
    }
}

void AMyCharacter::StopJump()
{
    // ジャンプボタンを離したときにジャンプ動作を終了
    StopJumping();
}
