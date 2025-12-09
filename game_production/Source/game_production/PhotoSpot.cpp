#include "PhotoSpot.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "MyCharacter.h"

APhotoSpot::APhotoSpot()
    : BestLocation(FVector::ZeroVector)
    , BestRotation(FRotator::ZeroRotator)
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
    if (!bPlayerInside)
    {
        UE_LOG(LogTemp, Warning, TEXT("📷 撮影失敗: スポット外"));
        return 0;
    }

    float Distance = FVector::Dist(CameraLocation, BestLocation);
    float DistanceRate = 1.0f - FMath::Clamp(Distance / MaxDistanceTolerance, 0.0f, 1.0f);

    float AngleDiff = FMath::Abs((CameraRotation - BestRotation).Yaw);
    float AngleRate = 1.0f - FMath::Clamp(AngleDiff / MaxAngleTolerance, 0.0f, 1.0f);

    float FinalRate = (DistanceRate + AngleRate) * 0.5f;
    int32 FinalScore = FMath::RoundToInt(MaxScore * FinalRate);

    UE_LOG(LogTemp, Warning, TEXT("📷 撮影成功! スコア: %d (距離率: %.2f, 角度率: %.2f)"),
        FinalScore, DistanceRate, AngleRate);

    return FinalScore;
}
