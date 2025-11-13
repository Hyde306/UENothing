#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhotoCameraActor.generated.h"

UCLASS()
class GAME_PRODUCTION_API APhotoCameraActor : public AActor
{
    GENERATED_BODY()

public:
    APhotoCameraActor();

    virtual void Tick(float DeltaTime) override;

    void SetCameraRotation(const FRotator& NewRot);
    void AddLookInput(float YawInput, float PitchInput);

    FVector GetCameraLocation() const;
    FRotator GetCameraRotation() const;
    FVector GetCameraForward() const;

    UCameraComponent* GetCameraComponent() const { return PhotoCamera; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* PhotoCamera;

    FRotator CurrentRotation;
};
