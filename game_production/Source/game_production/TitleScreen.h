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
    // ボタン
    UPROPERTY(meta = (BindWidget))
    class UButton* StartButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* BoyButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* GirlButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* ExitButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* HowToPlayButton;

    UFUNCTION()
    void OnHowToPlayClicked();

    UPROPERTY(meta = (BindWidget))
    class UButton* GameRulesButton;

    UFUNCTION()
    void OnGameRulesClicked();

    UFUNCTION()
    void OnExitClicked();
    // ボタンイベント
    UFUNCTION()
    void OnStartClicked();

    UFUNCTION()
    void OnBoyClicked();

    UFUNCTION()
    void OnGirlClicked();


};
