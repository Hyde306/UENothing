#include "TitleScreen.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"
#include "TimerManager.h"

void UTitleScreen::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UTitleScreen::OnStartClicked);
    }
    if (BoyButton)
    {
        BoyButton->OnClicked.AddDynamic(this, &UTitleScreen::OnBoyClicked);
    }
    if (GirlButton)
    {
        GirlButton->OnClicked.AddDynamic(this, &UTitleScreen::OnGirlClicked);
    }
    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UTitleScreen::OnExitClicked);
    }
    if (HowToPlayButton)
    {
        HowToPlayButton->OnClicked.AddDynamic(this, &UTitleScreen::OnHowToPlayClicked);
    }
    if (GameRulesButton)
    {
        GameRulesButton->OnClicked.AddDynamic(this,&UTitleScreen::OnGameRulesClicked);
    }
}
void UTitleScreen::OnStartClicked()
{
    PlaySoundAndOpenLevel("Dungeon");
}

void UTitleScreen::OnBoyClicked()
{
    PlayClickSound();

    SelectedCharacter = ECharacterSelect::Boy;

    if (UMyGameInstance* GI =
        Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        GI->SelectedCharacter = ECharacterVisual::Boy;
    }

    UpdateCharacterImages();
}

void UTitleScreen::OnGirlClicked()
{
    PlayClickSound();

    SelectedCharacter = ECharacterSelect::Girl;

    if (UMyGameInstance* GI =
        Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        GI->SelectedCharacter = ECharacterVisual::Girl;
    }

    UpdateCharacterImages();
}

void UTitleScreen::OnExitClicked()
{
    PlayClickSound();

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    UKismetSystemLibrary::QuitGame(
        GetWorld(),
        PC,
        EQuitPreference::Quit,
        false
    );
}

void UTitleScreen::OnHowToPlayClicked()
{
    PlaySoundAndOpenLevel("HowToPlay");
}


void UTitleScreen::OnGameRulesClicked()
{
    PlaySoundAndOpenLevel("GameRules");
}

void UTitleScreen::UpdateCharacterImages()
{
    if (BoyButton)
    {
        BoyButton->SetIsEnabled(SelectedCharacter != ECharacterSelect::Boy);
    }

    if (GirlButton)
    {
        GirlButton->SetIsEnabled(SelectedCharacter != ECharacterSelect::Girl);
    }
}

void UTitleScreen::PlayClickSound()
{
    if (ButtonClickSound)
    {
        UGameplayStatics::PlaySound2D(this, ButtonClickSound);
    }
}

void UTitleScreen::PlaySoundAndOpenLevel(FName LevelName)
{
    PlayClickSound();

    NextLevelName = LevelName;

    GetWorld()->GetTimerManager().SetTimer(
        TransitionTimer,
        this,
        &UTitleScreen::DelayedOpenLevel,
        0.25f,
        false
    );
}

void UTitleScreen::DelayedOpenLevel()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        PC->bShowMouseCursor = false;
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
    }

    UGameplayStatics::OpenLevel(this, NextLevelName);
}
