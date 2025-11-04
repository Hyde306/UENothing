#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsDoor.generated.h"

UCLASS()
class GAME_PRODUCTION_API APhysicsDoor : public AActor
{
	GENERATED_BODY()

public:
	APhysicsDoor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ルート、ドア枠、ドア
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Frame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Door;

	// --- 開閉設定 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	bool bIsOpen = false; // 開いてるかどうか

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	float OpenAngle = 90.0f; // 開く角度（負にすると逆方向）

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	float OpenSpeed = 2.0f; // 補間スピード

private:
	FRotator ClosedRotation;  // 閉じた時の回転
	FRotator TargetRotation;  // 補間先の角度
};
