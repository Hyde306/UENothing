#include "HowToPlayWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

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
    UGameplayStatics::OpenLevel(this, FName("Title"));
}
