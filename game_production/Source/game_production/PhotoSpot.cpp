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
    if (!bPlayerInside || !LinkedTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("📷 撮影失敗: スポット外またはターゲット未設定"));
        return 0;
    }

    /*
    // ↓ 距離・角度評価は今は使わないのでコメントアウト
    const FVector WorldBestLocation = BestLocation;
    const FRotator WorldBestRotation = BestRotation;

    float Distance = FVector::Distance(CameraLocation, WorldBestLocation);
    float DistanceRate = FMath::Clamp(1.0f - (Distance / MaxDistanceTolerance), 0.0f, 1.0f);

    FVector IdealForward = WorldBestRotation.Vector();
    FVector ActualForward = -CameraRotation.Vector();

    float Dot = FVector::DotProduct(IdealForward.GetSafeNormal(), ActualForward.GetSafeNormal());
    Dot = FMath::Clamp(Dot, -1.0f, 1.0f);
    float AngleDiff = FMath::RadiansToDegrees(FMath::Acos(Dot));
    float AngleRate = FMath::Clamp(1.0f - (AngleDiff / MaxAngleTolerance), 0.0f, 1.0f);

    float TotalRate = (AngleRate * 0.6f) + (DistanceRate * 0.4f);
    int32 FinalScore = FMath::RoundToInt(MaxScore * TotalRate);
    */

    UE_LOG(LogTemp, Warning, TEXT("📷 撮影成功! ターゲット '%s' を撮影しました"), *LinkedTarget->GetName());
    return MaxScore;
}
