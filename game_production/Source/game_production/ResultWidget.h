#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

UCLASS()
class GAME_PRODUCTION_API UResultWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(
        const FGeometry& MyGeometry,
        float InDeltaTime
    ) override;

    float RainbowTime = 0.0f;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Rank1Text;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Rank2Text;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Rank3Text;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Rank4Text;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Rank5Text;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Rank6Text;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TotalScoreText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TimeText;

public:
    void UpdateResult();
};
