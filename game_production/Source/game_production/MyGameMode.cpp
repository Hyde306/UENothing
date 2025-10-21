#include "MyGameMode.h"
#include "MyCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyGameMode::AMyGameMode()
{
    // デフォルトのポーン（プレイヤーキャラクター）を設定
    DefaultPawnClass = AMyCharacter::StaticClass();
}
