// PhotoTarget.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhotoTarget.generated.h"

UCLASS()
class GAME_PRODUCTION_API APhotoTarget : public AActor
{
    GENERATED_BODY()

public:
    APhotoTarget();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo")
    FString TargetName = "Unnamed Target";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo")
    int32 ScoreValue = 100;

    bool bAlreadyCaptured = false; // 撮影済みチェック

    // 注視点（ターゲットのどの部分を中心に見るか）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo")
    FVector FocusOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Photo Movement")
    float FloatAmplitude = 50.f; // 上下の振幅

    UPROPERTY(EditAnywhere, Category = "Photo Movement")
    float FloatSpeed = 2.f; // 周期の速さ

    float RunningTime = 0.f;

    UPROPERTY(EditAnywhere, Category = "Photo Movement")
    bool bEnableFloating = false; // ← 動かすかどうか

};
