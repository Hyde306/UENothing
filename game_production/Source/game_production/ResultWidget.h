#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;

UCLASS()
class GAME_PRODUCTION_API UResultWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void UpdateResult();
    void ShowNameInput();

    // –¼‘O“ü—ÍŠÖ˜A
    UFUNCTION()
    void OnNameChanged(const FText& Text);

    UFUNCTION()
    void OnDecideClicked();

    UFUNCTION()
    void OnExitClicked();

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TotalScoreText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank1Text;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank2Text;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank3Text;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank4Text;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank5Text;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rank6Text;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* NameInputBox;

    UPROPERTY(meta = (BindWidget))
    UButton* DecideButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ExitButton;

    UPROPERTY()
bool bNameInputShown = false;

private:
    float RainbowTime = 0.0f;
};
