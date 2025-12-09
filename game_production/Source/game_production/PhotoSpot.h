#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "PhotoSpot.generated.h"

UCLASS()
class GAME_PRODUCTION_API APhotoSpot : public AActor
{
    GENERATED_BODY()

public:
    APhotoSpot();

protected:
    virtual void BeginPlay() override;

    /** 撮影判定範囲 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PhotoSpot")
    class UBoxComponent* TriggerBox;

    /** 撮影中プレイヤーが範囲内にいるか */
    bool bPlayerInside = false;

public:
    /** スポット名 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    FString SpotName = TEXT("Unnamed Spot");

    /** 満点スコア */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    int32 MaxScore = 100;

    /** 撮影評価用：理想位置と角度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    FVector BestLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    FRotator BestRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    float MaxDistanceTolerance = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot|Scoring")
    float MaxAngleTolerance = 45.f;

public:
    /** 撮影範囲内か？ */
    bool CanTakePhoto() const { return bPlayerInside; }

    /** スポット名取得 */
    FString GetSpotName() const { return SpotName; }

    /** 撮影評価関数：スコアを返す */
    UFUNCTION(BlueprintCallable, Category = "PhotoSpot|Scoring")
    int32 EvaluatePhoto(const FVector& CameraLocation, const FRotator& CameraRotation) const;

    /** 撮影条件を満たしているか？（スポット内のみ） */
    UFUNCTION(BlueprintCallable, Category = "PhotoSpot|Scoring")
    bool IsValidPhoto() const { return bPlayerInside; }

protected:
    UFUNCTION()
    void OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
