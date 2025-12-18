#include "PhotoSpot.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

APhotoSpot::APhotoSpot()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    PhotoCenter = CreateDefaultSubobject<USceneComponent>(TEXT("PhotoCenter"));
    PhotoCenter->SetupAttachment(RootComponent);

    FramingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("FramingSphere"));
    FramingSphere->SetupAttachment(PhotoCenter);
    FramingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    FramingSphere->SetHiddenInGame(true);
}

void APhotoSpot::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (FramingSphere)
    {
        FramingSphere->SetSphereRadius(FramingRadius);
    }
}

void APhotoSpot::BeginPlay()
{
    Super::BeginPlay();
    StartLocation = GetActorLocation();
}

void APhotoSpot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bEnableMove)
        return;

    float Time = GetWorld()->GetTimeSeconds();
    float OffsetZ = FMath::Sin(Time * MoveSpeed) * MoveRange;

    FVector NewLocation = StartLocation;
    NewLocation.Z += OffsetZ;
    SetActorLocation(NewLocation);
}

int32 APhotoSpot::EvaluatePhoto(const FVector& CameraLocation)
{
    if (SuccessfulCaptureCount >= MaxCaptureCount)
        return -2;

    float Distance = FVector::Dist(CameraLocation, GetActorLocation());
    if (Distance > MaxDistanceTolerance)
        return -1;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
        return 0;

    int32 ScreenX, ScreenY;
    PC->GetViewportSize(ScreenX, ScreenY);

    FVector2D ScreenCenter(ScreenX / 2.0f, ScreenY / 2.0f);
    FVector CenterLoc = PhotoCenter->GetComponentLocation();

    FVector2D SpotCenter;
    PC->ProjectWorldLocationToScreen(CenterLoc, SpotCenter);

    float DistCenter = FVector2D::Distance(SpotCenter, ScreenCenter);
    float MaxDist = FVector2D(ScreenX, ScreenY).Size() * 0.5f;

    float CenterRate = FMath::Clamp(1.0f - DistCenter / MaxDist, 0.0f, 1.0f);
    int32 Score = FMath::RoundToInt(MaxScore * CenterRate);

    // 最高スコアを更新
    BestScore = FMath::Max(BestScore, Score);

    SuccessfulCaptureCount++;
    return Score;
}

bool APhotoSpot::IsFullyVisibleOnScreen(APlayerController* PC) const
{
    if (!PC)
        return false;

    int32 ScreenX, ScreenY;
    PC->GetViewportSize(ScreenX, ScreenY);

    FVector Base = PhotoCenter->GetComponentLocation();

    FVector Offsets[6] =
    {
        FVector(FramingRadius, 0, 0),
        FVector(-FramingRadius, 0, 0),
        FVector(0,  FramingRadius, 0),
        FVector(0, -FramingRadius, 0),
        FVector(0, 0,  FramingRadius),
        FVector(0, 0, -FramingRadius)
    };

    for (int i = 0; i < 6; i++)
    {
        FVector P = Base + Offsets[i];
        FVector2D S;
        bool Ok = PC->ProjectWorldLocationToScreen(P, S);

        if (!Ok)
            return false;

        if (S.X < 0 || S.X > ScreenX || S.Y < 0 || S.Y > ScreenY)
            return false;
    }

    return true;
}
