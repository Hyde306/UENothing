#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MyGameInstance.h"
#include "MyCharacter.generated.h"

UCLASS()
class GAME_PRODUCTION_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsJumping = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsInPhotoMode = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsTakingPhoto = false;

    // boyとgirlメッシュ選択
    UPROPERTY(EditDefaultsOnly)
    USkeletalMesh* BoyMesh;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UAnimInstance> BoyAnimBP;

    UPROPERTY(EditDefaultsOnly)
    USkeletalMesh* GirlMesh;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UAnimInstance> GirlAnimBP;

    UPROPERTY(BlueprintReadOnly, Category = "Photo")
    int32 SuccessfulPhotoCount = 0;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    USoundBase* ShutterSound;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;

    // 入力処理
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump();
    void StopJump();
    void StartRun();
    void StopRun();
    void TogglePhotoMode();
    void TakePhoto();
    void FinishGame();
    void OnRPressed();
    void OnRReleased();
    void TryRetry();
    void RetryGame();

    float DefaultFOV = 90.f;
    float PhotoFOV = 45.f;
    float ZoomInterpSpeed = 8.f;
    float TargetFOV = 90.f;

    bool bRKeyDown = false;
    bool bRetryTriggered = false;

    // Inputアクション
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputMappingContext* IMC_Player;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Move;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Jump;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Look;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Run;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_TogglePhotoMode;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_TakePhoto;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* IA_FinishGame;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_RKey;
    // カメラ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FollowCamera;

    // UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> CameraUIClass;

    UUserWidget* CameraUIInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PhotoResultWidgetClass;

    UUserWidget* PhotoResultWidgetInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> TimerUIClass;   // タイマーUI

    UUserWidget* TimerUIInstance;            // タイマーUIインスタンス
    UPROPERTY()
    class UTextBlock* TimerTextBlock;        // TimerText

    UPROPERTY()
    UTextBlock* PhotoCountText;
    // ===== Result UI =====
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UResultWidget> ResultWidgetClass;

    UPROPERTY()
    class UResultWidget* ResultWidgetInstance;

    // 撮影タイム計測
    UFUNCTION(BlueprintCallable, Category = "Photo")
    float GetPhotoElapsedTime() const { return PhotoElapsedTime; }
    
private:
    float PhotoStartTime = 0.f;
    float PhotoElapsedTime = 0.f;
};
