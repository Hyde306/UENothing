#include "TitleScreen.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UTitleScreen::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UTitleScreen::OnStartClicked);
    }
}
void UTitleScreen::OnStartClicked()
{
    // 入力モードをゲーム操作に戻す
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        PC->bShowMouseCursor = false;

        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
    }

    // レベル遷移
    UGameplayStatics::OpenLevel(this, FName("Dungeon"));
}
