#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class GAME_PRODUCTION_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // 撮影スポットごとの最高スコア
    UPROPERTY(BlueprintReadOnly)
    TMap<FString, int32> PhotoBestScores;

    // クリアタイム（秒）
    UPROPERTY(BlueprintReadOnly)
    float ClearTime = 0.0f;

public:
    // スコア更新（大きい方だけ保持）
    void UpdatePhotoScore(const FString& SpotName, int32 Score);

    // タイム記録
    void SetClearTime(float Time);

    // 合計スコア計算
    int32 GetTotalScore() const;
};
