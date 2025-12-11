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

public:
    /** スポット名 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    FString SpotName = TEXT("Unnamed Spot");

    /** 満点スコア */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    int32 MaxScore = 100;

    /** 撮影可能距離の上限 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    float MaxDistanceTolerance = 1000.f;

    /** 撮影評価関数：スコアを返す */
    UFUNCTION(BlueprintCallable, Category = "PhotoSpot|Scoring")
    int32 EvaluatePhoto(const FVector& CameraLocation); 

    UFUNCTION(BlueprintCallable, Category = "PhotoSpot")
    FString GetSpotName() const { return SpotName; }

    // 撮影成功回数
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PhotoSpot|Scoring")
    int32 SuccessfulCaptureCount = 0;

    // 撮影可能回数の上限
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    int32 MaxCaptureCount = 3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* PhotoCenter;
};
