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

    // ===== ì¸óÕèàóù =====
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump();
    void StopJump();
    void StartRun();
    void StopRun();

    // ===== Enhanced Input ê›íË =====
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

    // ===== ÉJÉÅÉâê›íË =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FollowCamera;

public:
    // AnimBPÇ…ExposeÇµÇƒÇ‡Ç¢Ç¢ÇÊÇ§Ç…BlueprintReadOnly
    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsRunning = false;
};
