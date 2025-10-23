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
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;

    // ========= 入力処理 =========
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump();
    void StopJump();
    void StartRun();
    void StopRun();

    // ========= Enhanced Input =========
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

    // ========= カメラ =========
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FollowCamera;

    // ========= アニメーション =========
    UPROPERTY(EditAnywhere, Category = "Animation")
    class UAnimMontage* IdleMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    class UAnimMontage* WalkMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    class UAnimMontage* RunMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    class UAnimMontage* JumpMontage;

    // ========= 状態管理 =========
    bool bIsMoving = false;
    bool bIsRunning = false;
    bool bIsJumping = false;

    void UpdateAnimation();
    void PlaySmoothAnimation(UAnimMontage* MontageToPlay, float BlendTime = 0.2f);
};
