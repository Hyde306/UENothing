#include "PhotoTarget.h"

APhotoTarget::APhotoTarget()
{
    PrimaryActorTick.bCanEverTick = true;
    FloatAmplitude = 200.f; // 振幅
    FloatSpeed = 2.f;      // 速さ
    RunningTime = 0.f;
}

void APhotoTarget::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bEnableFloating) return; // 動かさないターゲットはここで終了

    RunningTime += DeltaTime;
    FVector NewLocation = GetActorLocation();
    NewLocation.Z += FMath::Sin(RunningTime * FloatSpeed) * FloatAmplitude * DeltaTime;
    SetActorLocation(NewLocation);
}