#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleScreen.generated.h"

UCLASS()
class GAME_PRODUCTION_API UTitleScreen : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    // Blueprint ‚Ìƒ{ƒ^ƒ“‚ð•R•t‚¯‚é
    UPROPERTY(meta = (BindWidget))
    class UButton* StartButton;

    UFUNCTION()
    void OnStartClicked();
};
