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