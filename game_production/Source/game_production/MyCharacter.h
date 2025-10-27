#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyCharacter.generated.h"

UCLASS()
class GAME_PRODUCTION_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // ===== 入力処理 =====
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump();
    void StopJump();
    void StartRun();
    void StopRun();
    void StartTakePhoto();  // ← 撮影アニメーション開始

    // ===== Enhanced Input =====
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

    // クリック撮影用 InputAction
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_TakePhoto;

    // ===== カメラ =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FollowCamera;

public:
    // ===== キャラクター状態 =====
    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsRunning = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsInPhotoMode = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsJumping = false;

    // ★撮影中フラグ
    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsTakingPhoto = false;
};
