#include "HowToPlayWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UHowToPlayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(
            this,
            &UHowToPlayWidget::OnBackClicked
        );
    }
}

void UHowToPlayWidget::OnBackClicked()
{
    PlaySoundAndOpenLevel("Title");
}

void UHowToPlayWidget::PlaySoundAndOpenLevel(FName LevelName)
{
    if (ButtonClickSound)
    {
        UGameplayStatics::PlaySound2D(this, ButtonClickSound);
    }

    NextLevelName = LevelName;

    GetWorld()->GetTimerManager().SetTimer(
        TransitionTimer,
        this,
        &UHowToPlayWidget::DelayedOpenLevel,
        0.25f,
        false
    );
}

void UHowToPlayWidget::DelayedOpenLevel()
{
    UGameplayStatics::OpenLevel(this, NextLevelName);
}