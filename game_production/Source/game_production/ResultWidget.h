#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

UCLASS(Blueprintable)
class GAME_PRODUCTION_API UResultWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void UpdateResult();

protected:
    // UMG‘¤‚Ì TextBlock ‚ÆŒ‹‚Ñ•t‚¯‚é
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TotalScoreText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TimeText;
};