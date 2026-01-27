#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "HowToPlayWidget.generated.h"

UCLASS()
class GAME_PRODUCTION_API UHowToPlayWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UButton* BackButton;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    USoundBase* ButtonClickSound;

    FTimerHandle TransitionTimer;
    FName NextLevelName;

    UFUNCTION()
    void OnBackClicked();

    UFUNCTION()
    void PlaySoundAndOpenLevel(FName LevelName);

    UFUNCTION()
    void DelayedOpenLevel();
};