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

    // 撮影判定範囲
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PhotoSpot")
    class UBoxComponent* TriggerBox;

public:
    // このスポットで撮影できるターゲット
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    class APhotoTarget* LinkedTarget;

    // スポット名やスコア
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    FString SpotName = "Unnamed Spot";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhotoSpot")
    int32 ScoreValue = 100;

protected:
    // プレイヤーが範囲内か
    bool bPlayerInside = false;

public:
    // トリガーイベント
    UFUNCTION()
    void OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // 撮影判定
    bool CanTakePhoto() const { return bPlayerInside; }

    int32 GetScore() const { return ScoreValue; }
    FString GetSpotName() const { return SpotName; }
};
