#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundBase.h"
#include "TitleScreen.generated.h"

// 選択状態
UENUM(BlueprintType)
enum class ECharacterSelect : uint8
{
    None,
    Boy,
    Girl
};

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

    UPROPERTY(meta = (BindWidget))
    class UButton* GameRulesButton;
    // クリック音
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    USoundBase* ButtonClickSound;

    // 選択状態
    ECharacterSelect SelectedCharacter = ECharacterSelect::None;

    // 次に遷移するレベル名
    FName NextLevelName;

    // タイマー
    FTimerHandle TransitionTimer;

    // ボタンイベント
    UFUNCTION()
    void OnStartClicked();

    UFUNCTION()
    void OnBoyClicked();

    UFUNCTION()
    void OnGirlClicked();

    UFUNCTION()
    void OnHowToPlayClicked();

    UFUNCTION()
    void OnGameRulesClicked();

    UFUNCTION()
    void OnExitClicked();

    UFUNCTION()
    void UpdateCharacterImages();

    // 共通処理
    UFUNCTION()
    void PlayClickSound();

    UFUNCTION()
    void PlaySoundAndOpenLevel(FName LevelName);

    UFUNCTION()
    void DelayedOpenLevel();
};
