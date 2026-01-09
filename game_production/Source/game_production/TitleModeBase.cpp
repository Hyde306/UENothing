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

            // “ü—Íƒ‚[ƒh‚ð UI ê—p‚ÉØ‚è‘Ö‚¦
            APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
            if (PC)
            {
                PC->bShowMouseCursor = true;

                FInputModeUIOnly InputMode;
                PC->SetInputMode(InputMode);
            }
        }
    }
}
