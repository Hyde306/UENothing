#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

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
void UMyGameInstance::AddScoreToRanking(int32 FinalScore)
{
    ScoreRanking.Add(FinalScore);

    ScoreRanking.Sort([](int32 A, int32 B)
        {
            return A > B;
        });

    if (ScoreRanking.Num() > 6)
    {
        ScoreRanking.SetNum(6);
    }

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
void UMyGameInstance::SaveRanking()
{
    UMySaveGame* SaveData =
        Cast<UMySaveGame>(
            UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass())
        );

    if (!SaveData) return;

    SaveData->ScoreRanking = ScoreRanking;

    UGameplayStatics::SaveGameToSlot(
        SaveData,
        SaveSlotName,
        SaveUserIndex
    );
}