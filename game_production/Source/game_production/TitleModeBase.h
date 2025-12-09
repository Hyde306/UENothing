#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TitleModeBase.generated.h"

UCLASS()
class GAME_PRODUCTION_API ATitleModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

protected:
    // タイトル画面用ウィジェットクラス（Blueprintで指定）
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> TitleScreenClass;

private:
    UPROPERTY()
    class UUserWidget* TitleScreenWidget;
};
