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

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return 0;

    // 注視点のワールド座標を取得
    FVector FocusWorldLocation = LinkedTarget->GetActorTransform().TransformPosition(LinkedTarget->FocusOffset);

    // スクリーン座標に変換
    FVector2D ScreenLocation;
    bool bProjected = PC->ProjectWorldLocationToScreen(FocusWorldLocation, ScreenLocation);

    if (!bProjected)
    {
        UE_LOG(LogTemp, Warning, TEXT("📷 撮影失敗: 注視点が画面外"));
        return 0;
    }

    int32 ScreenX, ScreenY;
    PC->GetViewportSize(ScreenX, ScreenY);
    FVector2D ScreenCenter(ScreenX / 2.0f, ScreenY / 2.0f);

    float DistanceFromCenter = FVector2D::Distance(ScreenLocation, ScreenCenter);
    float MaxScreenDistance = FVector2D(ScreenX, ScreenY).Size() / 2.0f;

    float CenterRate = FMath::Clamp(1.0f - (DistanceFromCenter / MaxScreenDistance), 0.0f, 1.0f);
    int32 FinalScore = FMath::RoundToInt(MaxScore * CenterRate);

    UE_LOG(LogTemp, Warning, TEXT("📷 撮影成功! スコア: %d (中心率: %.2f)"), FinalScore, CenterRate);
    return FinalScore;
}
