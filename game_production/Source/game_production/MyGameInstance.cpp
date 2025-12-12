#include "MyGameInstance.h"

void UMyGameInstance::UpdatePhotoScore(const FString& SpotName, int32 Score)
{
    int32* Prev = PhotoBestScores.Find(SpotName);

    if (!Prev || Score > *Prev)
    {
        PhotoBestScores.Add(SpotName, Score);
    }
}

void UMyGameInstance::SetClearTime(float Time)
{
    ClearTime = Time;
}

int32 UMyGameInstance::GetTotalScore() const
{
    int32 Total = 0;
    for (auto& Elem : PhotoBestScores)
    {
        Total += Elem.Value;
    }
    return Total;
}
