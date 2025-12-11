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

        TargetFOV = PhotoFOV; // ← ズームイン開始！

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

        TargetFOV = DefaultFOV; // ← ズームアウト開始！

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

    FVector Forward = FollowCamera->GetForwardVector();

    FHitResult Hit;
    FVector Start = CameraLocation;
    FVector End = Start + Forward * 1500.0f;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    APhotoSpot* LookAtSpot = nullptr;

    if (bHit)
    {
        LookAtSpot = Cast<APhotoSpot>(Hit.GetActor());
    }

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (LookAtSpot)
    {
        bool bFramed = LookAtSpot->IsFullyVisibleOnScreen(PC);

        if (!bFramed)
        {
            LookAtSpot = nullptr;
        }
    }

    if (LookAtSpot)
    {
        int32 Score = LookAtSpot->EvaluatePhoto(CameraLocation);

        if (Score > 0)
        {
            HitSpot = LookAtSpot;
            HitScore = Score;
        }
    }

    FString ResultMessage;
    FLinearColor FlashColor;

    if (HitSpot)
    {
        ResultMessage = FString::Printf(
            TEXT("%s 撮影成功！ スコア: %d"),
            *HitSpot->GetSpotName(), HitScore
        );
        FlashColor = FLinearColor::White;
    }
    else
    {
        ResultMessage = TEXT("撮影失敗");
        FlashColor = FLinearColor::Red;
    }

    if (PhotoResultWidgetClass)
    {
        UPhotoResultWidget* Widget =
            CreateWidget<UPhotoResultWidget>(GetWorld(), PhotoResultWidgetClass);

        if (Widget)
        {
            Widget->AddToViewport();
            Widget->SetVisibility(ESlateVisibility::Visible);
            Widget->SetResultText(ResultMessage);

            FTimerHandle FadeHandle;
            GetWorldTimerManager().SetTimer(
                FadeHandle,
                [Widget]()
                {
                    Widget->PlayFadeOut();
                },
                2.0f,
                false
            );
        }
    }

    FString ScreenshotName = FString::Printf(
        TEXT("Photo_%s.png"),
        *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"))
    );

    FString SavePath = FPaths::ProjectSavedDir() + "Screenshots/" + ScreenshotName;

    FScreenshotRequest::RequestScreenshot(SavePath, false, false);

    FTimerHandle FlashHandle;
    FLinearColor FlashCopy = FlashColor;

    GetWorldTimerManager().SetTimer(
        FlashHandle,
        [this, FlashCopy]()
        {
            if (APlayerController* PC = Cast<APlayerController>(GetController()))
            {
                if (APlayerCameraManager* CamMgr = PC->PlayerCameraManager)
                {
                    CamMgr->StartCameraFade(0.f, 1.f, 0.15f, FlashCopy, false, true);

                    FTimerHandle Handle2;
                    GetWorldTimerManager().SetTimer(
                        Handle2,
                        [CamMgr, FlashCopy]()
                        {
                            CamMgr->StartCameraFade(1.f, 0.f, 0.8f, FlashCopy, false, true);
                        },
                        0.3f,
                        false
                    );
                }
            }
        },
        0.1f,
        false
    );

    FTimerHandle ResetHandle;
    GetWorldTimerManager().SetTimer(
        ResetHandle,
        [this]()
        {
            bIsTakingPhoto = false;
        },
        1.0f,
        false
    );
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
