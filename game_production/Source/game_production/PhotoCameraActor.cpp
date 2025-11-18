#include "PhotoCameraActor.h"

// ============================
// コンストラクタ
// ============================
APhotoCameraActor::APhotoCameraActor()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    RootComponent = SpringArm;

    SpringArm->TargetArmLength = 0.f;
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bDoCollisionTest = false;

    PhotoCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PhotoCamera"));
    PhotoCamera->SetupAttachment(SpringArm);
}

// ============================
// BeginPlay
// ============================
void APhotoCameraActor::BeginPlay()
{
    Super::BeginPlay();
}

// ============================
// Tick
// ============================
void APhotoCameraActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    SpringArm->SetWorldRotation(CurrentRotation);
}

// ============================
// 外部から回転をセット
// ============================
void APhotoCameraActor::SetCameraRotation(const FRotator& NewRot)
{
    CurrentRotation = NewRot;
}

// ============================
// Look 入力を加える
// ============================
void APhotoCameraActor::AddLookInput(float YawInput, float PitchInput)
{
    CurrentRotation.Yaw += YawInput;
    CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch + PitchInput, -80.f, 80.f);
}

FVector APhotoCameraActor::GetCameraLocation() const
{
    return PhotoCamera ? PhotoCamera->GetComponentLocation() : FVector::ZeroVector;
}

FRotator APhotoCameraActor::GetCameraRotation() const
{
    return PhotoCamera ? PhotoCamera->GetComponentRotation() : FRotator::ZeroRotator;
}

FVector APhotoCameraActor::GetCameraForward() const
{
    return PhotoCamera ? PhotoCamera->GetForwardVector() : FVector::ForwardVector;
}
