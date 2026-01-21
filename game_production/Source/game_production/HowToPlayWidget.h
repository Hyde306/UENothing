#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HowToPlayWidget.generated.h"

UCLASS()
class GAME_PRODUCTION_API UHowToPlayWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UButton* BackButton;

    UFUNCTION()
    void OnBackClicked();
};