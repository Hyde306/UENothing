#include "MyGameMode.h"
#include "MyCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyGameMode::AMyGameMode()
{
    // �f�t�H���g�̃|�[���i�v���C���[�L�����N�^�[�j��ݒ�
    DefaultPawnClass = AMyCharacter::StaticClass();
}
