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
    // すでにワールド座標で設定されているので、そのまま使う
    const FVector WorldBestLocation = BestLocation;
    const FRotator WorldBestRotation = BestRotation;

    float Distance = FVector::Distance(CameraLocation, WorldBestLocation);

    FVector IdealForward = WorldBestRotation.Vector();
    FVector ActualForward = CameraRotation.Vector();
    float Dot = FVector::DotProduct(IdealForward, ActualForward);
    float AngleDiff = FMath::RadiansToDegrees(acosf(FMath::Clamp(Dot, -1.0f, 1.0f)));

    float DistanceRate = FMath::Clamp(1.0f - (Distance / MaxDistanceTolerance), 0.0f, 1.0f);
    float AngleRate = FMath::Clamp(1.0f - (AngleDiff / MaxAngleTolerance), 0.0f, 1.0f);
    float TotalRate = (AngleRate * 0.6f) + (DistanceRate * 0.4f);
    int32 FinalScore = FMath::RoundToInt(MaxScore * TotalRate);

    UE_LOG(LogTemp, Warning, TEXT("🎯 Spot '%s' 評価結果 ==="), *SpotName);
    UE_LOG(LogTemp, Warning, TEXT("CameraLocation : %s"), *CameraLocation.ToString());
    UE_LOG(LogTemp, Warning, TEXT("WorldBestLocation : %s"), *WorldBestLocation.ToString());
    UE_LOG(LogTemp, Warning, TEXT("距離差 : %.2f cm, 角度差 : %.2f°, スコア : %d点"),
        Distance, AngleDiff, FinalScore);

    return FinalScore;
}
