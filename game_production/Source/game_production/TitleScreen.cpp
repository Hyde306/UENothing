#include "TitleScreen.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"

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
    // “ü—Íƒ‚[ƒh‚ðƒQ[ƒ€‘€ì‚É–ß‚·
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        PC->bShowMouseCursor = false;

        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
    }

    // ƒŒƒxƒ‹‘JˆÚ
    UGameplayStatics::OpenLevel(this, FName("Dungeon"));
}

void UTitleScreen::OnBoyClicked()
{
    if (UMyGameInstance* GI =
        Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        GI->SelectedCharacter = ECharacterVisual::Boy;
    }
}

void UTitleScreen::OnGirlClicked()
{
    if (UMyGameInstance* GI =
        Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        GI->SelectedCharacter = ECharacterVisual::Girl;
    }
}

void UTitleScreen::OnExitClicked()
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

void UTitleScreen::OnHowToPlayClicked()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        PC->bShowMouseCursor = true;

        FInputModeUIOnly InputMode;
        PC->SetInputMode(InputMode);
    }

    UGameplayStatics::OpenLevel(this, FName("HowToPlay"));
}

void UTitleScreen::OnGameRulesClicked()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        PC->bShowMouseCursor = true;

        FInputModeUIOnly InputMode;
        PC->SetInputMode(InputMode);
    }

    UGameplayStatics::OpenLevel(this, FName("GameRules"));
}