#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyCharacter.generated.h"

UCLASS()
class GAME_PRODUCTION_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // �R���X�g���N�^
    // �R���|�[�l���g�̏������Ȃǂ��s��
    AMyCharacter();

protected:
    // �Q�[���J�n���܂��̓X�|�[�����ɌĂ΂��
    virtual void BeginPlay() override;

    // �v���C���[���͂�ݒ肷��֐�
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ==========================
    // ���͏����֘A
    // ==========================

    // �ړ����͏���
    // WASD �� ���X�e�B�b�N�̓��͒l���󂯎��A�L�����N�^�[���ړ�������
    void Move(const FInputActionValue& Value);

    // ���_���쏈��
    // �}�E�X��E�X�e�B�b�N�̓��͒l���󂯎��A�J�����̌�����ύX����
    void Look(const FInputActionValue& Value);

    // �W�����v�J�n����
    // �W�����v�{�^���������ꂽ�Ƃ��ɌĂ΂��
    void StartJump();

    // �W�����v�I������
    // �W�����v�{�^���������ꂽ�Ƃ��ɌĂ΂��
    void StopJump();

protected:
    // ==========================
    // Enhanced Input �֘A
    // ==========================

    // �v���C���[�p�̓��̓}�b�s���O�R���e�L�X�g
    // �e�A�N�V�����i�ړ��E�W�����v�E���_����Ȃǁj��R�Â���ݒ�
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputMappingContext* IMC_Player;

    // �ړ��A�N�V�����iWASD���͂Ȃǁj
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Move;

    // �W�����v�A�N�V�����i�X�y�[�X�L�[�Ȃǁj
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Jump;

    // ���_����A�N�V�����i�}�E�X�E�E�X�e�B�b�N�j
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Look;

    // ==========================
    // �J�����֘A�R���|�[�l���g
    // ==========================

    // �J�����p�̃X�v�����O�A�[��
    // �L�����N�^�[�����苗����ۂ��ăJ������Ǐ]������
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    // ���ۂɕ\�������J����
    // �X�v�����O�A�[���̐�[�ɃA�^�b�`����A�v���C���[���_�𐧌䂷��
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FollowCamera;
};
