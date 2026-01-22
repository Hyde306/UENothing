#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MyGameInstance.h"   // ← FRankingData を使うために必要
#include "MySaveGame.generated.h"

UCLASS()
class GAME_PRODUCTION_API UMySaveGame : public USaveGame
{
    GENERATED_BODY()

public:

    // ランキング保存用
    UPROPERTY(BlueprintReadWrite)
    TArray<FRankingData> ScoreRanking;
};
