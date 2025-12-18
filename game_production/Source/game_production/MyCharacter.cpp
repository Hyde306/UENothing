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
#include "PhotoResultWidget.h"
#include "MyGameInstance.h"
#include "ResultWidget.h"

// ===========================
// コンストラクタ
// ===========================
AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.f;
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bDoCollisionTest = false;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->JumpZVelocity = 420.f;
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

// ===========================
// BeginPlay
// ===========================
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (UMyGameInstance* GI =
        Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        if (GI->SelectedCharacter == ECharacterVisual::Boy)
        {
            GetMesh()->SetSkeletalMesh(BoyMesh);
            GetMesh()->SetAnimInstanceClass(BoyAnimBP);
        }
        else
        {
            GetMesh()->SetSkeletalMesh(GirlMesh);
            GetMesh()->SetAnimInstanceClass(GirlAnimBP);
        }
    }

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (IMC_Player)
                Subsystem->AddMappingContext(IMC_Player, 0);
        }
    }

    PhotoStartTime = GetWorld()->GetTimeSeconds();

    if (TimerUIClass)
    {
        TimerUIInstance = CreateWidget<UUserWidget>(GetWorld(), TimerUIClass);
        if (TimerUIInstance)
        {
            TimerUIInstance->AddToViewport();
            TimerTextBlock = Cast<UTextBlock>(TimerUIInstance->GetWidgetFromName(TEXT("TimerText")));

        }
    }
    if (TimerUIInstance)
    {
        TimerTextBlock =
            Cast<UTextBlock>(TimerUIInstance->GetWidgetFromName(TEXT("TimerText")));

        PhotoCountText =
            Cast<UTextBlock>(TimerUIInstance->GetWidgetFromName(TEXT("PhotoCountText")));
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

        if (IA_FinishGame)
        {
            EnhancedInput->BindAction(IA_FinishGame, ETriggerEvent::Started, this, &AMyCharacter::FinishGame);
        }

    }
}

// ===========================
// 移動
// ===========================
void AMyCharacter::Move(const FInputActionValue& Value)
{
    if (bIsInPhotoMode) return;

    FVector2D Input = Value.Get<FVector2D>();
    if (Controller && (Input.X != 0.f || Input.Y != 0.f))
    {
        FRotator ControlRot = Controller->GetControlRotation();
        FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

        FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        FVector MoveDir = (Forward * Input.Y) + (Right * Input.X);

        AddMovementInput(MoveDir.GetSafeNormal());

        if (!MoveDir.IsNearlyZero())
        {
            FRotator TargetRot = MoveDir.Rotation();
            FRotator SmoothRot = FMath::RInterpTo(GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 10.f);
            SetActorRotation(SmoothRot);
        }
    }
}

// ===========================
// Look 操作
// ===========================
void AMyCharacter::Look(const FInputActionValue& Value)
{
    FVector2D Axis = Value.Get<FVector2D>();

    // 通常は Controller を回す
    if (Controller)
    {
        float NewPitch = Controller->GetControlRotation().Pitch + Axis.Y;
        NewPitch = FMath::ClampAngle(NewPitch, -89.f, 89.f);


        float NewYaw = Controller->GetControlRotation().Yaw + Axis.X;

        FRotator NewRot(NewPitch, NewYaw, 0.0f);
        Controller->SetControlRotation(NewRot);
    }
}

// ===========================
// フォトモード切替
// ===========================
void AMyCharacter::TogglePhotoMode()
{
    bIsInPhotoMode = !bIsInPhotoMode;

    if (bIsInPhotoMode)
    {
        // 一人称フォトモード
        GetMesh()->SetOwnerNoSee(true);
        GetCharacterMovement()->DisableMovement();

        SpringArm->TargetArmLength = 0.f;
        SpringArm->bUsePawnControlRotation = true;
        FollowCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

        TargetFOV = PhotoFOV; 

        if (CameraUIClass && !CameraUIInstance)
        {
            CameraUIInstance = CreateWidget<UUserWidget>(GetWorld(), CameraUIClass);
            if (CameraUIInstance) CameraUIInstance->AddToViewport();
        }
        if (CameraUIInstance)
            CameraUIInstance->SetVisibility(ESlateVisibility::Visible);

        UE_LOG(LogTemp, Warning, TEXT("PhotoMode ON (1st Person)"));
    }
    else
    {
        // 三人称へ戻る
        GetMesh()->SetOwnerNoSee(false);
        GetCharacterMovement()->SetMovementMode(MOVE_Walking);

        SpringArm->TargetArmLength = 300.f;
        SpringArm->bUsePawnControlRotation = true;
        FollowCamera->SetRelativeLocation(FVector::ZeroVector);

        TargetFOV = DefaultFOV; 

        if (CameraUIInstance)
            CameraUIInstance->SetVisibility(ESlateVisibility::Hidden);

        UE_LOG(LogTemp, Warning, TEXT("PhotoMode OFF (3rd Person)"));
    }
}

// ===========================
// 撮影処理
// ===========================
void AMyCharacter::TakePhoto()
{
    if (!bIsInPhotoMode || bIsTakingPhoto) return;
    bIsTakingPhoto = true;

    FVector CameraLocation = FollowCamera->GetComponentLocation();
    APhotoSpot* HitSpot = nullptr;
    int32 HitScore = 0;

    // カメラ前方方向
    FVector Forward = FollowCamera->GetForwardVector();

    FHitResult Hit;
    FVector Start = CameraLocation;
    FVector End = Start + Forward * 1500.0f;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    APhotoSpot* LookAtSpot = nullptr;

    if (bHit)
    {
        LookAtSpot = Cast<APhotoSpot>(Hit.GetActor());
    }

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC)
    {
        bIsTakingPhoto = false;
        return;
    }

    if (LookAtSpot && !LookAtSpot->IsFullyVisibleOnScreen(PC))
    {
        LookAtSpot = nullptr;
    }

    // 撮影成功時のスコア計算
    if (LookAtSpot)
    {
        int32 Score = LookAtSpot->EvaluatePhoto(CameraLocation);

        if (Score > 0)
        {
            HitSpot = LookAtSpot;
            HitScore = Score;

            // GameInstance に最高スコアを更新
            if (UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
            {
                GI->UpdatePhotoScore(HitSpot->GetSpotName(), HitScore);
            }
        }
    }

    // 撮影結果メッセージとフラッシュ色
    FString ResultMessage;
    FLinearColor FlashColor;
    if (HitSpot)
    {
        ResultMessage = FString::Printf(TEXT("%s 撮影成功！\n         スコア: %d"), *HitSpot->GetSpotName(), HitScore);
        FlashColor = FLinearColor::White;
    }
    else
    {
        ResultMessage = TEXT("                  撮影失敗");
        FlashColor = FLinearColor::Red;
    }

    // UI表示
    if (PhotoResultWidgetClass)
    {
        UPhotoResultWidget* Widget = CreateWidget<UPhotoResultWidget>(GetWorld(), PhotoResultWidgetClass);
        if (Widget)
        {
            Widget->AddToViewport();
            Widget->SetVisibility(ESlateVisibility::Visible);
            Widget->SetResultText(ResultMessage);

            FTimerHandle FadeHandle;
            GetWorldTimerManager().SetTimer(FadeHandle, [Widget]()
                {
                    Widget->PlayFadeOut();
                }, 2.0f, false);
        }
    }

    // スクリーンショット保存
    FString ScreenshotName = FString::Printf(TEXT("Photo_%s.png"), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
    FString SavePath = FPaths::ProjectSavedDir() + "Screenshots/" + ScreenshotName;
    FScreenshotRequest::RequestScreenshot(SavePath, false, false);

    // フラッシュ演出
    FTimerHandle FlashHandle;
    FLinearColor FlashCopy = FlashColor;
    GetWorldTimerManager().SetTimer(FlashHandle, [this, FlashCopy]()
        {
            if (APlayerController* PC = Cast<APlayerController>(GetController()))
            {
                if (APlayerCameraManager* CamMgr = PC->PlayerCameraManager)
                {
                    CamMgr->StartCameraFade(0.f, 1.f, 0.15f, FlashCopy, false, true);

                    FTimerHandle Handle2;
                    GetWorldTimerManager().SetTimer(Handle2, [CamMgr, FlashCopy]()
                        {
                            CamMgr->StartCameraFade(1.f, 0.f, 0.8f, FlashCopy, false, true);
                        }, 0.3f, false);
                }
            }
        }, 0.1f, false);

    // 撮影可能状態のリセット
    FTimerHandle ResetHandle;
    GetWorldTimerManager().SetTimer(ResetHandle, [this]()
        {
            bIsTakingPhoto = false;
        }, 1.0f, false);
}


void AMyCharacter::FinishGame()
{
    UE_LOG(LogTemp, Warning, TEXT("FinishGame CALLED"));

    UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GI) return;

    // クリアタイム保存
    GI->SetClearTime(PhotoElapsedTime);

    // 撮影スコア合計
    int32 PhotoScore = GI->GetTotalScore();

    // タイムスコア（15秒ごとに10点減点）
    int32 ElapsedTime = FMath::FloorToInt(PhotoElapsedTime);
    int32 TimeScore = 600 - (ElapsedTime / 15) * 10;
    TimeScore = FMath::Max(TimeScore, 0);

    // 最終スコア確定
    int32 FinalScore = PhotoScore + TimeScore;

    // ランキングに登録
    GI->AddScoreToRanking(FinalScore);

    // リザルトUI表示
    if (ResultWidgetClass)
    {
        ResultWidgetInstance = CreateWidget<UResultWidget>(GetWorld(), ResultWidgetClass);
        if (ResultWidgetInstance)
        {
            ResultWidgetInstance->AddToViewport();
        }
    }

    // 入力をUIに
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        PC->bShowMouseCursor = true;

        FInputModeUIOnly Mode;
        PC->SetInputMode(Mode);
    }
}


// ===========================
// Tick
// ===========================
void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float CurrentFOV = FollowCamera->FieldOfView;
    float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, ZoomInterpSpeed);
    FollowCamera->SetFieldOfView(NewFOV);

    PhotoElapsedTime = GetWorld()->GetTimeSeconds() - PhotoStartTime;

    if (TimerTextBlock)
    {
        int32 Minutes = FMath::FloorToInt(PhotoElapsedTime / 60.f);
        int32 Seconds = FMath::FloorToInt(FMath::Fmod(PhotoElapsedTime, 60.f));
        FString TimerStr = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        TimerTextBlock->SetText(FText::FromString(TimerStr));
    }

    if (PhotoCountText)
    {
        if (UMyGameInstance* GI =
            Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
        {
            int32 Count = GI->GetCapturedPhotoCount();
            FString Str = FString::Printf(TEXT("Photo: %d / 10"), Count);
            PhotoCountText->SetText(FText::FromString(Str));
        }
    }

}

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
