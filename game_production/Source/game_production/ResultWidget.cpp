#include "ResultWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"

void UResultWidget::NativeConstruct()
{
    Super::NativeConstruct();

    TotalScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TotalScoreText")));
    TimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TimeText")));

    UpdateResult();
}


void UResultWidget::UpdateResult()
{
    UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GI) return;

    int32 TotalScore = GI->GetTotalScore();
    int32 ElapsedTime = FMath::FloorToInt(GI->ClearTime); // タイムを整数化

    // タイムをスコアに換算
    int32 TimeScore = 600 - (ElapsedTime / 15) * 10;
    TimeScore = FMath::Max(TimeScore, 0); // 最低スコアは0

    int32 CombinedScore = TotalScore + TimeScore;

    if (TotalScoreText)
    {
        FString ScoreString = FString::Printf(TEXT("Score: %d"), CombinedScore);
        TotalScoreText->SetText(FText::FromString(ScoreString));
    }

    if (TimeText)
    {
        FString TimeString = FString::Printf(TEXT("Time: %d"), ElapsedTime);
        TimeText->SetText(FText::FromString(TimeString));
    }
}
