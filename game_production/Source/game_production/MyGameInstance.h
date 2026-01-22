#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

class UMySaveGame;

UENUM(BlueprintType)
enum class ECharacterVisual : uint8
{
    Boy,
    Girl
};

// 名前付きランキング用構造体（必ず GameInstance より先に）
USTRUCT(BlueprintType)
struct FRankingData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PlayerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Score;
};

UCLASS()
class GAME_PRODUCTION_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    ECharacterVisual SelectedCharacter = ECharacterVisual::Boy;

    UPROPERTY(BlueprintReadOnly)
    TMap<FString, int32> PhotoBestScores;

    UPROPERTY(BlueprintReadOnly)
    float ClearTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    TArray<FRankingData> ScoreRanking;

    UPROPERTY(BlueprintReadOnly)
    int32 LastFinalScore = 0;

    UFUNCTION(BlueprintCallable)
    void ResetPhotoScores();

    void UpdatePhotoScore(const FString& SpotName, int32 Score);

    void AddScoreToRanking(const FString& PlayerName, int32 FinalScore);

    void SetClearTime(float Time);

    int32 GetTotalScore() const;

    int32 GetCapturedPhotoCount() const;

    bool IsRankIn(int32 NewScore) const;

protected:
    virtual void Init() override;

private:
    void LoadRanking();
    void SaveRanking();

    const FString SaveSlotName = TEXT("RankingSave");
    const int32 SaveUserIndex = 0;
};
