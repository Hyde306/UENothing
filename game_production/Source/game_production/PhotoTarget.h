// PhotoTarget.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhotoTarget.generated.h"

UCLASS()
class GAME_PRODUCTION_API APhotoTarget : public AActor
{
    GENERATED_BODY()

public:
    APhotoTarget();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo")
    FString TargetName = "Unnamed Target";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photo")
    int32 ScoreValue = 100;

    bool bAlreadyCaptured = false; // 撮影済みチェック
};
