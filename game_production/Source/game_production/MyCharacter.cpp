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
#include "PhotoTarget.h"
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
        SpringArm->bUsePawnControlRotation = true;   // ★★ 必ず true

        FollowCamera->SetRelativeLocation(FVector::ZeroVector);

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
    if (!bIsInPhotoMode) return;
    if (bIsTakingPhoto) return;

    bIsTakingPhoto = true;

    FVector CameraLocation = FollowCamera->GetComponentLocation();
    FRotator CameraRotation = FollowCamera->GetComponentRotation();
    FVector Start = CameraLocation;
    FVector End = Start + FollowCamera->GetForwardVector() * 5000.f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    APhotoTarget* HitTarget = bHit ? Cast<APhotoTarget>(Hit.GetActor()) : nullptr;

    bool bScored = false;
    FLinearColor FlashColor = FLinearColor::Red;

    if (HitTarget)
    {
        if (!HitTarget->bAlreadyCaptured)
            HitTarget->bAlreadyCaptured = true;

        TArray<AActor*> FoundSpots;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APhotoSpot::StaticClass(), FoundSpots);


        APhotoSpot* CapturedSpot = nullptr;
        int32 Score = 0;


        for (AActor* Actor : FoundSpots)
        {
            APhotoSpot* Spot = Cast<APhotoSpot>(Actor);
            if (Spot && Spot->CanTakePhoto() && Spot->LinkedTarget == HitTarget)
            {
                Score = Spot->EvaluatePhoto(CameraLocation, CameraRotation);
                CapturedSpot = Spot;

                bScored = true;
                FlashColor = FLinearColor::White;
                break;
            }
        }

        if (bScored && CapturedSpot)
        {
            if (PhotoResultWidgetClass)
            {
                UE_LOG(LogTemp, Warning, TEXT("PhotoResultWidgetClass は設定されています"));

                UPhotoResultWidget* Widget = CreateWidget<UPhotoResultWidget>(GetWorld(), PhotoResultWidgetClass);
                if (Widget)
                {
                    UE_LOG(LogTemp, Warning, TEXT("ウィジェット作成成功"));
                    Widget->AddToViewport();
                    Widget->SetResultText(FString::Printf(TEXT("%sのフォトスポット撮影成功！\n\n                   スコア: %d"), *CapturedSpot->SpotName, Score));

                    // 2秒後にフェードアウト開始
                    FTimerHandle FadeHandle;
                    GetWorldTimerManager().SetTimer(FadeHandle, [Widget]()
                        {
                            Widget->PlayFadeOut();
                        }, 2.0f, false);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("ウィジェット作成に失敗しました"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("PhotoResultWidgetClass が設定されていません"));
            }
        }


    }

    // スクリーンショット
    FString ScreenshotName = FString::Printf(TEXT("Photo_%s.png"),
        *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
    FString SavePath = FPaths::ProjectSavedDir() + "Screenshots/" + ScreenshotName;
    FScreenshotRequest::RequestScreenshot(SavePath, false, false);

    // フラッシュ
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (APlayerCameraManager* CamMgr = PC->PlayerCameraManager)
        {
            CamMgr->StartCameraFade(0.f, 1.f, 0.15f, FlashColor, false, true);

            FTimerHandle Handle;
            GetWorldTimerManager().SetTimer(Handle, [CamMgr, FlashColor]()
                {
                    CamMgr->StartCameraFade(1.f, 0.f, 0.8f, FlashColor, false, true);
                }, 0.3f, false);
        }
    }

    // クールタイム
    FTimerHandle ResetHandle;
    GetWorldTimerManager().SetTimer(ResetHandle, [this]()
        {
            bIsTakingPhoto = false;
        }, 1.0f, false);
}

// ===========================
// Tick
// ===========================
void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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
