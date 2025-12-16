#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

UCLASS()
class GAME_PRODUCTION_API UMySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<int32> ScoreRanking;
};
