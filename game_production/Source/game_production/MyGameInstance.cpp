#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MySaveGame.h"

// =======================
// 起動時
// =======================
void UMyGameInstance::Init()
{
    Super::Init();

#if !WITH_EDITOR
    LoadRanking();
#endif
}

// =======================
// 撮影スポット最高スコア更新
// =======================
void UMyGameInstance::UpdatePhotoScore(const FString& SpotName, int32 Score)
{
    int32* Prev = PhotoBestScores.Find(SpotName);

    if (!Prev || Score > *Prev)
    {
        PhotoBestScores.Add(SpotName, Score);
    }
}

// =======================
// クリアタイム保存
// =======================
void UMyGameInstance::SetClearTime(float Time)
{
    ClearTime = Time;
}

// =======================
// 合計撮影スコア
// =======================
int32 UMyGameInstance::GetTotalScore() const
{
    int32 Total = 0;
    for (const auto& Elem : PhotoBestScores)
    {
        Total += Elem.Value;
    }
    return Total;
}

// =======================
// ランキング追加
// =======================
void UMyGameInstance::AddScoreToRanking(const FString& PlayerName, int32 FinalScore)
{
    FRankingData NewData;
    NewData.PlayerName = PlayerName;
    NewData.Score = FinalScore;

    ScoreRanking.Add(NewData);

    ScoreRanking.Sort([](const FRankingData& A, const FRankingData& B)
        {
            return A.Score > B.Score;
        });

    if (ScoreRanking.Num() > 6)
        ScoreRanking.SetNum(6);

#if !WITH_EDITOR
    SaveRanking();
#endif
}

// =======================
// ロード
// =======================
void UMyGameInstance::LoadRanking()
{
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
    {
        return;
    }

    USaveGame* Loaded =
        UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex);

    UMySaveGame* SaveData = Cast<UMySaveGame>(Loaded);
    if (!SaveData) return;

    ScoreRanking = SaveData->ScoreRanking;
}

// =======================
// セーブ
// =======================
int32 UMyGameInstance::GetCapturedPhotoCount() const
{
    return PhotoBestScores.Num();
}

void UMyGameInstance::ResetPhotoScores()
{
    PhotoBestScores.Empty();   // 撮影スコアを全部消す
    ClearTime = 0.0f;
}

bool UMyGameInstance::IsRankIn(int32 NewScore) const
{
    int32 Count = 0;
    for (const FRankingData& Data : ScoreRanking)
    {
        if (NewScore <= Data.Score)
            Count++;
    }
    return Count < 6;
}

void UMyGameInstance::SaveRanking()
{
    UMySaveGame* SaveData = Cast<UMySaveGame>(
        UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass())
    );

    if (!SaveData) return;

    // ランキングを保存
    SaveData->ScoreRanking = ScoreRanking;

    // セーブ
    UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, SaveUserIndex);
}
