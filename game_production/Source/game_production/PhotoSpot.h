#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhotoSpot.generated.h"

class USphereComponent;
class USceneComponent;

UCLASS()
class GAME_PRODUCTION_API APhotoSpot : public AActor
{
    GENERATED_BODY()

public:
    APhotoSpot();
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    FString SpotName = TEXT("Unnamed Spot");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    int32 MaxCaptureCount = 3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PhotoSpot")
    int32 SuccessfulCaptureCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    float MaxDistanceTolerance = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    int32 MaxScore = 100;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PhotoSpot")
    USceneComponent* PhotoCenter;

    UFUNCTION(BlueprintCallable, Category = "PhotoSpot")
    FString GetSpotName() const { return SpotName; }

    UFUNCTION(BlueprintCallable, Category = "PhotoSpot")
    int32 EvaluatePhoto(const FVector& CameraLocation);

    UFUNCTION(BlueprintCallable, Category = "PhotoSpot")
    bool IsFullyVisibleOnScreen(APlayerController* PC) const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    float FramingRadius = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PhotoSpot")
    USphereComponent* FramingSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    bool bEnableMove = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    float MoveRange = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    float MoveSpeed = 2.0f;

private:
    FVector StartLocation;
};
