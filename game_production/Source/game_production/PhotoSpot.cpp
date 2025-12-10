#include "PhotoSpot.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

APhotoSpot::APhotoSpot()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APhotoSpot::BeginPlay()
{
    Super::BeginPlay();
}

int32 APhotoSpot::EvaluatePhoto(const FVector& CameraLocation)
{
    // 撮影回数上限チェック
    if (SuccessfulCaptureCount >= MaxCaptureCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("📷 撮影失敗: %s はこれ以上撮影できません"), *SpotName);
        return -2; // 特別な失敗コード
    }

    // 距離チェック
    float Distance = FVector::Dist(CameraLocation, GetActorLocation());
    if (Distance > MaxDistanceTolerance)
    {
        UE_LOG(LogTemp, Warning, TEXT("📷 撮影失敗: 距離が遠すぎる (%.1f)"), Distance);
        return -1; // 距離失敗
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return 0;

    // バウンディングボックス判定（省略）

    // 中心判定
    int32 ScreenX, ScreenY;
    PC->GetViewportSize(ScreenX, ScreenY);
    FVector2D ScreenCenter(ScreenX / 2.0f, ScreenY / 2.0f);
    FVector2D SpotCenter;
    PC->ProjectWorldLocationToScreen(GetActorLocation(), SpotCenter);

    float DistanceFromCenter = FVector2D::Distance(SpotCenter, ScreenCenter);
    float MaxScreenDistance = FVector2D(ScreenX, ScreenY).Size() / 2.0f;

    float CenterRate = FMath::Clamp(1.0f - (DistanceFromCenter / MaxScreenDistance), 0.0f, 1.0f);
    int32 FinalScore = FMath::RoundToInt(MaxScore * CenterRate);

    // 成功したらカウントを増やす
    SuccessfulCaptureCount++;

    UE_LOG(LogTemp, Warning, TEXT("📷 撮影成功! スコア: %d (中心率: %.2f) 残り撮影可能回数: %d"),
        FinalScore, CenterRate, MaxCaptureCount - SuccessfulCaptureCount);

    return FinalScore;
}
