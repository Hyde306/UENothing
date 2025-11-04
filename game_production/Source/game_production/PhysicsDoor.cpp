#include "PhysicsDoor.h"
#include "Components/StaticMeshComponent.h"

APhysicsDoor::APhysicsDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	// コンポーネント構成
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Frame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame"));
	Frame->SetupAttachment(RootComponent);

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(Frame);
}

void APhysicsDoor::BeginPlay()
{
	Super::BeginPlay();

	ClosedRotation = Door->GetRelativeRotation();
	TargetRotation = ClosedRotation;
}

void APhysicsDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 開閉アニメーション（補間）
	FRotator CurrentRot = Door->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRotation, DeltaTime, OpenSpeed);
	Door->SetRelativeRotation(NewRot);

	// ターゲット回転を切り替える
	if (bIsOpen)
	{
		TargetRotation = ClosedRotation + FRotator(0.f, OpenAngle, 0.f);
	}
	else
	{
		TargetRotation = ClosedRotation;
	}
}
