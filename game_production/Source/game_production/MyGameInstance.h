#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MySaveGame.h"
#include "MyGameInstance.generated.h"

UENUM(BlueprintType)
enum class ECharacterVisual : uint8
{
    Boy,
    Girl
};

UCLASS()
class GAME_PRODUCTION_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    ECharacterVisual SelectedCharacter = ECharacterVisual::Boy;

    // 撮影スポットごとの最高スコア
    UPROPERTY(BlueprintReadOnly)
    TMap<FString, int32> PhotoBestScores;

    // クリアタイム（秒）
    UPROPERTY(BlueprintReadOnly)
    float ClearTime = 0.0f;

    // スコアランキング（高い順）
    UPROPERTY(BlueprintReadOnly)
    TArray<int32> ScoreRanking;

    UFUNCTION(BlueprintCallable)
    void ResetPhotoScores();

    // 撮影スコア更新
    void UpdatePhotoScore(const FString& SpotName, int32 Score);

    // ランキングにスコアを追加
    void AddScoreToRanking(int32 FinalScore);

    // タイム記録
    void SetClearTime(float Time);

    // 合計スコア
    int32 GetTotalScore() const;

    int32 GetCapturedPhotoCount() const;

protected:
    // 起動時（ここでロード）
    virtual void Init() override;

private:
    void LoadRanking();
    void SaveRanking();

    const FString SaveSlotName = TEXT("RankingSave");
    const int32 SaveUserIndex = 0;

};
