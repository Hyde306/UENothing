#include "ResultWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"

void UResultWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);

    UpdateResult();
}

void UResultWidget::UpdateResult()
{
    UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GI) return;

    int32 TotalScore = GI->GetTotalScore();
    float Time = GI->ClearTime;

    if (TotalScoreText)
    {
        TotalScoreText->SetText(FText::AsNumber(TotalScore));
    }

    if (TimeText)
    {
        // •b ¨ ¬”‘æ1ˆÊ‚Ü‚Å
        FString TimeString = FString::Printf(TEXT("%.1f"), Time);
        TimeText->SetText(FText::FromString(TimeString));
    }
}
