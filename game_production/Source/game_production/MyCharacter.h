#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
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
    virtual void Tick(float DeltaTime) override;

    // ===== ì¸óÕèàóù =====
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump();
    void StopJump();
    void StartRun();
    void StopRun();
    void TogglePhotoMode();
    void TakePhoto();

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

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_TogglePhotoMode;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_TakePhoto;

    // ===== ÉJÉÅÉâ =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FollowCamera;

    // ===== UI =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    TSubclassOf<UUserWidget> CameraUIClass;

    UUserWidget* CameraUIInstance;

public:
    // ===== èÛë‘ =====
    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsRunning = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsJumping = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsInPhotoMode = false;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsTakingPhoto = false;
};
