#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhotoSpot.generated.h"

UCLASS()
class GAME_PRODUCTION_API APhotoSpot : public AActor
{
    GENERATED_BODY()

public:
    APhotoSpot();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    FString SpotName = TEXT("Unnamed Spot");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    int32 MaxScore = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    float MaxDistanceTolerance = 1000.f;

    UFUNCTION(BlueprintCallable, Category = "PhotoSpot|Scoring")
    int32 EvaluatePhoto(const FVector& CameraLocation);

    UFUNCTION(BlueprintCallable, Category = "PhotoSpot")
    FString GetSpotName() const { return SpotName; }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PhotoSpot|Scoring")
    int32 SuccessfulCaptureCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    int32 MaxCaptureCount = 3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* PhotoCenter;

    // -------------------------
    // 上下動のための値
    // -------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Move")
    bool bEnableMove = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Move")
    float MoveRange = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Move")
    float MoveSpeed = 2.0f;

private:
    FVector StartLocation;
};
