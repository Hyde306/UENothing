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

void UResultWidget::NativeTick(
    const FGeometry& MyGeometry,
    float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    RainbowTime += InDeltaTime;

    if (Rank1Text)
    {
        // 色相を回す（0〜360）
        float Hue = FMath::Fmod(RainbowTime * 120.f, 360.f);

        FLinearColor RainbowColor =
            FLinearColor::MakeFromHSV8(
                (uint8)(Hue / 360.f * 255.f),
                255,
                255
            );

        Rank1Text->SetColorAndOpacity(RainbowColor);
    }
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
        TotalScoreText->SetColorAndOpacity(FLinearColor(FColor(255, 255, 0)));
    }

    if (TimeText)
    {
        FString TimeString = FString::Printf(TEXT("Time: %d"), ElapsedTime);
        TimeText->SetText(FText::FromString(TimeString));
        TimeText->SetColorAndOpacity(FLinearColor(FColor(0, 0, 255)));
    }
    const TArray<int32>& Ranking = GI->ScoreRanking;

    UTextBlock* RankTexts[6] =
    {
        Rank1Text, Rank2Text, Rank3Text,
        Rank4Text, Rank5Text, Rank6Text
    };

    for (int32 i = 0; i < 6; i++)
    {
        if (!RankTexts[i]) continue;

        if (Ranking.IsValidIndex(i))
        {
            FString RankString =
                FString::Printf(TEXT("%d : %d"), i + 1, Ranking[i]);
            RankTexts[i]->SetText(FText::FromString(RankString));

            // 順位ごとの装飾
            FSlateFontInfo Font = RankTexts[i]->Font;

            if (i == 0) // 1位
            {
                Font.Size = 90;
                RankTexts[i]->SetFont(Font);
            }
            else if (i == 1) // 2位
            {
                Font.Size = 90;
                RankTexts[i]->SetColorAndOpacity(FLinearColor(FColor(46, 204, 113)));//シルバー
            }
            else if (i == 2) // 3位
            {
                Font.Size = 90;
                RankTexts[i]->SetColorAndOpacity(FLinearColor(FColor(169, 7, 228)));//紫
            }
            else
            {
                Font.Size = 90;
                RankTexts[i]->SetColorAndOpacity(FLinearColor(FColor(255, 255, 255)));//白
            }

            RankTexts[i]->SetFont(Font);
        }
        else
        {
            RankTexts[i]->SetText(FText::FromString(TEXT("-")));
        }
    }

}
