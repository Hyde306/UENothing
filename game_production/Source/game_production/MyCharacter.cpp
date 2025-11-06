#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "HighResScreenshot.h"
#include "PhotoSpot.h"
#include"PhotoTarget.h"

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

    //  カメラが壁や床に当たって距離が変わらないようにする
    SpringArm->bDoCollisionTest = false;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm);
    FollowCamera->bUsePawnControlRotation = false;

    // ===== フォトモード用カメラ =====
    PhotoCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PhotoCamera"));
    PhotoCamera->SetupAttachment(GetMesh());
    PhotoCamera->SetRelativeLocation(FVector(0.f, 20.f, 160.f));
    PhotoCamera->SetActive(false);

    // ===== キャラ回転設定 =====
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false; // ★ カメラではなく入力方向で回す
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

    // ===== 空中制御設定 =====
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->JumpZVelocity = 420.f;

    // ===== 初期速度 =====
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
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

        if (IA_TogglePhotoMode)
            EnhancedInput->BindAction(IA_TogglePhotoMode, ETriggerEvent::Started, this, &AMyCharacter::TogglePhotoMode);

        if (IA_TakePhoto)
            EnhancedInput->BindAction(IA_TakePhoto, ETriggerEvent::Started, this, &AMyCharacter::TakePhoto);
    }
}

// ===========================
// 移動処理
// ===========================
void AMyCharacter::Move(const FInputActionValue& Value)
{
    if (bIsInPhotoMode) return;

    FVector2D Input = Value.Get<FVector2D>();
    if (Controller && (Input.X != 0.f || Input.Y != 0.f))
    {
        // カメラの向きを取得（Yawのみ）
        FRotator ControlRot = Controller->GetControlRotation();
        FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

        // 移動方向を算出
        FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        FVector MoveDir = (Forward * Input.Y) + (Right * Input.X);

        // 実際に移動
        AddMovementInput(MoveDir.GetSafeNormal());

        // キャラクターを移動方向に回転
        if (!MoveDir.IsNearlyZero())
        {
            FRotator TargetRot = MoveDir.Rotation();
            FRotator SmoothRot = FMath::RInterpTo(GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 10.f);
            SetActorRotation(SmoothRot);
        }
    }
}

// ===========================
// カメラ操作
// ===========================
void AMyCharacter::Look(const FInputActionValue& Value)
{
    FVector2D Axis = Value.Get<FVector2D>();

    if (bIsInPhotoMode)
    {
        // ===== フォトモード時はカメラのみ回す（今のままでOK） =====
        PhotoCameraRotation.Yaw += Axis.X;
        PhotoCameraRotation.Pitch = FMath::Clamp(PhotoCameraRotation.Pitch + Axis.Y, -80.f, 80.f);
        PhotoCamera->SetRelativeRotation(PhotoCameraRotation);
    }
    else
    {
        // ===== 通常モード（三人称） =====
        if (Controller)
        {
            // ※ここで「マウス上＝上を向く」方向に直す
            float NewPitch = Controller->GetControlRotation().Pitch + Axis.Y;

            // ピッチの範囲制限（真上・真下を向きすぎない）
            NewPitch = FMath::ClampAngle(NewPitch, -60.f, 60.f);

            // Yaw（左右）は制限なし
            float NewYaw = Controller->GetControlRotation().Yaw + Axis.X;

            // 新しい回転を適用
            FRotator NewRot(NewPitch, NewYaw, 0.0f);
            Controller->SetControlRotation(NewRot);
        }
    }
}

// ジャンプ
void AMyCharacter::StartJump()
{
    if (bIsInPhotoMode) return;

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
    if (bIsInPhotoMode) return;

    bIsRunning = true;
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AMyCharacter::StopRun()
{
    bIsRunning = false;
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

// ===========================
// フォトモード切替
// ===========================
void AMyCharacter::TogglePhotoMode()
{
    bIsInPhotoMode = !bIsInPhotoMode;

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (bIsInPhotoMode)
        {
            // === フォトモード ON ===
            FollowCamera->SetActive(false);
            PhotoCamera->SetActive(true);
            // キャラクターのMeshを非表示にして手を消す
            GetMesh()->SetOwnerNoSee(true);

            PC->SetViewTargetWithBlend(this, 0.3f);
            UE_LOG(LogTemp, Warning, TEXT("Photo Mode ON"));

            PC->SetViewTargetWithBlend(this, 0.3f);
            UE_LOG(LogTemp, Warning, TEXT("Photo Mode ON"));

            // === UIを表示 ===
            if (CameraUIClass && !CameraUIInstance)
            {
                CameraUIInstance = CreateWidget<UUserWidget>(GetWorld(), CameraUIClass);
                if (CameraUIInstance)
                {
                    CameraUIInstance->AddToViewport();
                }
            }
            if (CameraUIInstance)
            {
                CameraUIInstance->SetVisibility(ESlateVisibility::Visible);
            }
        }
        else
        {
            // === フォトモード OFF ===
            PhotoCamera->SetActive(false);
            FollowCamera->SetActive(true);
            GetMesh()->SetOwnerNoSee(false);

            // ★ フォトモードFOVをリセット（次回ズーム演出を再度できるように）
            PhotoCamera->SetFieldOfView(NormalFOV);

            PC->SetViewTargetWithBlend(this, 0.3f);
            UE_LOG(LogTemp, Warning, TEXT("Photo Mode OFF"));

            if (CameraUIInstance)
            {
                CameraUIInstance->SetVisibility(ESlateVisibility::Hidden);
                UE_LOG(LogTemp, Warning, TEXT("Camera UI Hidden"));
            }
        }


    }
}

// ===========================
// 撮影処理
// ===========================
void AMyCharacter::TakePhoto()
{
    if (!bIsInPhotoMode) return;
    if (bIsTakingPhoto) return;

    bIsTakingPhoto = true;
    UE_LOG(LogTemp, Warning, TEXT("📸 撮影開始！"));

    // === カメラ前方にレイを飛ばす ===
    FVector Start = PhotoCamera->GetComponentLocation();
    FVector End = Start + (PhotoCamera->GetForwardVector() * 5000.0f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    APhotoTarget* HitTarget = bHit ? Cast<APhotoTarget>(Hit.GetActor()) : nullptr;

    // === 撮影スコア判定用 ===
    bool bScored = false;
    FLinearColor FlashColor = FLinearColor::Red; // デフォルト：赤（失敗）

    if (HitTarget)
    {
        if (!HitTarget->bAlreadyCaptured)
        {
            HitTarget->bAlreadyCaptured = true;
            UE_LOG(LogTemp, Warning, TEXT("🎯 '%s' 撮影記録（初撮影）"), *HitTarget->TargetName);
        }

        // 撮影スポットの検索
        TArray<AActor*> FoundSpots;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APhotoSpot::StaticClass(), FoundSpots);

        for (AActor* Actor : FoundSpots)
        {
            APhotoSpot* Spot = Cast<APhotoSpot>(Actor);
            if (Spot && Spot->CanTakePhoto() && Spot->LinkedTarget == HitTarget)
            {
                UE_LOG(LogTemp, Warning, TEXT("💎 Spot '%s' で '%s' を撮影成功！ +%d点"),
                    *Spot->GetSpotName(), *HitTarget->TargetName, Spot->GetScore());
                bScored = true;
                FlashColor = FLinearColor::White; // 成功時：白フラッシュ
                break;
            }
        }

        if (!bScored)
        {
            UE_LOG(LogTemp, Warning, TEXT("⚠️ スポット外撮影（'%s'）: スコア無効"), *HitTarget->TargetName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ 何もヒットしませんでした。"));
    }

    // === スクリーンショット保存 ===
    FString ScreenshotName = FString::Printf(TEXT("Photo_%s.png"),
        *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
    FString SavePath = FPaths::ProjectSavedDir() + "Screenshots/" + ScreenshotName;
    FScreenshotRequest::RequestScreenshot(SavePath, false, false);
    UE_LOG(LogTemp, Warning, TEXT("💾 スクリーンショット保存: %s"), *SavePath);

    // === 撮影後のフラッシュを 0.3 秒遅らせて発動 ===
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (APlayerCameraManager* CamMgr = PC->PlayerCameraManager)
        {
            FTimerHandle FlashHandle;
            GetWorldTimerManager().SetTimer(FlashHandle, [this, CamMgr, FlashColor]()
                {
                    CamMgr->StartCameraFade(0.f, 1.f, 0.15f, FlashColor, false, true);

                    FTimerHandle FadeHandle;
                    // ← thisをキャプチャしているのでGetWorld()が使える
                    this->GetWorld()->GetTimerManager().SetTimer(FadeHandle, [CamMgr, FlashColor]()
                        {
                            CamMgr->StartCameraFade(1.f, 0.f, 0.8f, FlashColor, false, true);
                        }, 0.3f, false);

                }, 0.3f, false); // フラッシュ発動まで0.3秒遅延
        }
    }

    // === クールタイム ===
    FTimerHandle ResetHandle;
    GetWorldTimerManager().SetTimer(ResetHandle, [this]()
        {
            bIsTakingPhoto = false;
            UE_LOG(LogTemp, Warning, TEXT("📸 撮影終了"));
        }, 1.0f, false);
}



void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // === 現在のターゲットFOV ===
    float TargetFOV = bIsInPhotoMode ? PhotoModeFOV : NormalFOV;

    UCameraComponent* ActiveCam = nullptr;

    if (FollowCamera && FollowCamera->IsActive())
        ActiveCam = FollowCamera;
    else if (PhotoCamera && PhotoCamera->IsActive())
        ActiveCam = PhotoCamera;

    if (ActiveCam)
    {
        float NewFOV = FMath::FInterpTo(
            ActiveCam->FieldOfView,  // 現在のFOV
            TargetFOV,               // 目標FOV
            DeltaTime,               // 経過時間
            5.0f                     // ← ここでズーム速度を調整
        );
        ActiveCam->SetFieldOfView(NewFOV);
    }
}
