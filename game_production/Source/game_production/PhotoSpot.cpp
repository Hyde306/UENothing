#include "PhotoSpot.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "MyCharacter.h"

APhotoSpot::APhotoSpot()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;
    TriggerBox->InitBoxExtent(FVector(200.f, 200.f, 100.f));
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void APhotoSpot::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APhotoSpot::OnPlayerEnter);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APhotoSpot::OnPlayerExit);
}

void APhotoSpot::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (Cast<AMyCharacter>(OtherActor))
    {
        bPlayerInside = true;
        UE_LOG(LogTemp, Warning, TEXT("Player entered photo spot: %s"), *SpotName);
    }
}

void APhotoSpot::OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (Cast<AMyCharacter>(OtherActor))
    {
        bPlayerInside = false;
        UE_LOG(LogTemp, Warning, TEXT("Player left photo spot: %s"), *SpotName);
    }
}

int32 APhotoSpot::EvaluatePhoto(const FVector& CameraLocation, const FRotator& CameraRotation) const
{
    // === 距離によるスコア（0〜1） ===
    float Distance = FVector::Distance(CameraLocation, BestLocation);
    float DistanceRate = FMath::Clamp(1.0f - (Distance / MaxDistanceTolerance), 0.0f, 1.0f);

    // === 角度によるスコア（Yaw のみで判定） ===
    float IdealYaw = BestRotation.Yaw;
    float ActualYaw = CameraRotation.Yaw;

    // Yaw差を-180〜180°の範囲で計算（最小の差を取る）
    float AngleDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(IdealYaw, ActualYaw));
    float AngleRate = FMath::Clamp(1.0f - (AngleDiff / MaxAngleTolerance), 0.0f, 1.0f);

    // === スコア合成（角度60％、距離40％） ===
    float TotalRate = (AngleRate * 0.6f) + (DistanceRate * 0.4f);
    int32 FinalScore = FMath::RoundToInt(MaxScore * TotalRate);

    // === デバッグ出力 ===
    UE_LOG(LogTemp, Warning, TEXT("📷 Spot '%s' 評価結果 → 距離差: %.1fcm, 角度差: %.1f°, スコア: %d点"),
        *SpotName, Distance, AngleDiff, FinalScore);

    return FinalScore;
}
