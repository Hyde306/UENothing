#include "TitleModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ATitleModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (TitleScreenClass)
    {
        TitleScreenWidget = CreateWidget<UUserWidget>(GetWorld(), TitleScreenClass);
        if (TitleScreenWidget)
        {
            TitleScreenWidget->AddToViewport();

            // 入力モードを UI 専用に切り替え（フォーカス設定は不要）
            APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
            if (PC)
            {
                PC->bShowMouseCursor = true;

                FInputModeUIOnly InputMode;
                PC->SetInputMode(InputMode); // ← SetWidgetToFocus は削除
            }
        }
    }
}
