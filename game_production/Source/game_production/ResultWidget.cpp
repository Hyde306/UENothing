#include "ResultWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyGameInstance.h"

void UResultWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ScoreSound)
    {
        UGameplayStatics::PlaySound2D(this, ScoreSound);
    }

    if (ExitButton)
        ExitButton->OnClicked.AddDynamic(this, &UResultWidget::OnExitClicked);

    if (DecideButton)
    {
        DecideButton->OnClicked.AddDynamic(this, &UResultWidget::OnDecideClicked);
        DecideButton->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (NameInputBox)
    {
        NameInputBox->OnTextChanged.AddDynamic(this, &UResultWidget::OnNameChanged);
        NameInputBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &UResultWidget::OnBackClicked);
    }

    UpdateResult();
}

void UResultWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    RainbowTime += InDeltaTime;

    if (Rank1Text)
    {
        float Hue = FMath::Fmod(RainbowTime * 120.f, 360.f);
        FLinearColor RainbowColor = FLinearColor::MakeFromHSV8(
            (uint8)(Hue / 360.f * 255.f), 255, 255);
        Rank1Text->SetColorAndOpacity(RainbowColor);
    }
    if (Rank2Text)
    {
        Rank2Text->SetColorAndOpacity(FLinearColor(0.2f, 0.4f, 1.0f)); // 青
    }

    if (Rank3Text)
    {
        Rank3Text->SetColorAndOpacity(FLinearColor(0.2f, 1.0f, 0.3f)); // 緑
    }
}

void UResultWidget::UpdateResult()
{
    UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GI) return;

    int32 TotalScore = GI->GetTotalScore();
    int32 ElapsedTime = FMath::FloorToInt(GI->ClearTime);

    int32 TimeScore = 600 - (ElapsedTime / 15) * 10;
    TimeScore = FMath::Max(TimeScore, 0);

    int32 CombinedScore = TotalScore + TimeScore;
    GI->LastFinalScore = CombinedScore;

    if (TotalScoreText)
        TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), CombinedScore)));

    if (TimeText)
        TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %d"), ElapsedTime)));

    // 名前入力は6位以内かつ未表示の場合のみ
    if (GI->IsRankIn(CombinedScore) && !bNameInputShown)
    {
        ShowNameInput();
        bNameInputShown = true;
    }

    // ランキング表示は既存のランキングのみ
    const TArray<FRankingData>& Ranking = GI->ScoreRanking;
    UTextBlock* RankTexts[6] = { Rank1Text, Rank2Text, Rank3Text, Rank4Text, Rank5Text, Rank6Text };

    for (int32 i = 0; i < 6; i++)
    {
        if (!RankTexts[i]) continue;

        if (Ranking.IsValidIndex(i))
        {
            FString RankString = FString::Printf(TEXT("%d.%s  %d"),
                i + 1, *Ranking[i].PlayerName, Ranking[i].Score);
            RankTexts[i]->SetText(FText::FromString(RankString));
        }
        else
        {
            RankTexts[i]->SetText(FText::FromString(TEXT("-")));
        }
    }
}

void UResultWidget::ShowNameInput()
{
    if (NameInputBox && DecideButton)
    {
        NameInputBox->SetVisibility(ESlateVisibility::Visible);
        DecideButton->SetVisibility(ESlateVisibility::Visible);
        NameInputBox->SetText(FText::FromString(TEXT("")));
    }
}

void UResultWidget::OnNameChanged(const FText& Text)
{
    if (!NameInputBox) return;

    FString Input = Text.ToString();
    FString Filtered;

    int32 Width = 0;
    int32 JapaneseCount = 0;

    for (TCHAR C : Input)
    {
        // 半角英数字（ASCII）
        bool bIsHalfWidth = (C >= 0x21 && C <= 0x7E);

        // 絵文字（禁止）
        bool bIsEmoji = (C >= 0x1F300 && C <= 0x1FAFF);
        if (bIsEmoji)
        {
            continue;
        }

        // 全角判定（ASCII 以外は全部全角扱い）
        bool bIsFullWidth = !bIsHalfWidth;

        int32 CharWidth = bIsFullWidth ? 2 : 1;

        // 日本語（全角）は最大3文字
        if (bIsFullWidth)
        {
            if (JapaneseCount >= 3)
            {
                continue;
            }
        }

        // 幅6を超えるなら追加しない
        if (Width + CharWidth > 6)
        {
            break;
        }

        Filtered.AppendChar(C);
        Width += CharWidth;

        if (bIsFullWidth)
        {
            JapaneseCount++;
        }
    }

    if (Filtered != Input)
    {
        NameInputBox->SetText(FText::FromString(Filtered));
    }
}


void UResultWidget::OnDecideClicked()
{
    if (ButtonClickSound)
    {
        UGameplayStatics::PlaySound2D(this, ButtonClickSound);
    }

    if (!NameInputBox) return;

    FString PlayerName = NameInputBox->GetText().ToString();
    if (PlayerName.Len() == 0) return;

    UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GI) return;

    // 決定時にのみランキング追加
    GI->AddScoreToRanking(PlayerName, GI->LastFinalScore);

    NameInputBox->SetVisibility(ESlateVisibility::Collapsed);
    DecideButton->SetVisibility(ESlateVisibility::Collapsed);

    UpdateResult(); // UI更新
}

void UResultWidget::OnExitClicked()
{
    PlaySoundAndQuit();
}

void UResultWidget::PlaySoundAndQuit()
{
    if (ButtonClickSound)
    {
        UGameplayStatics::PlaySound2D(this, ButtonClickSound);
    }

    GetWorld()->GetTimerManager().SetTimer(
        TransitionTimer,
        this,
        &UResultWidget::DelayedQuit,
        0.25f,
        false
    );
}

void UResultWidget::DelayedQuit()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    UKismetSystemLibrary::QuitGame(
        GetWorld(),
        PC,
        EQuitPreference::Quit,
        false
    );
}

void UResultWidget::OnBackClicked()
{
    PlaySoundAndOpenLevel("Title");
}

void UResultWidget::PlaySoundAndOpenLevel(FName LevelName)
{
    if (ButtonClickSound)
    {
        UGameplayStatics::PlaySound2D(this, ButtonClickSound);
    }

    NextLevelName = LevelName;

    GetWorld()->GetTimerManager().SetTimer(
        TransitionTimer,
        this,
        &UResultWidget::DelayedOpenLevel,
        0.25f,
        false
    );
}

void UResultWidget::DelayedOpenLevel()
{
    UGameplayStatics::OpenLevel(this, NextLevelName);
}