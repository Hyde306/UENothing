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
    // コンストラクタ
    // コンポーネントの初期化などを行う
    AMyCharacter();

protected:
    // ゲーム開始時またはスポーン時に呼ばれる
    virtual void BeginPlay() override;

    // プレイヤー入力を設定する関数
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ==========================
    // 入力処理関連
    // ==========================

    // 移動入力処理
    // WASD や 左スティックの入力値を受け取り、キャラクターを移動させる
    void Move(const FInputActionValue& Value);

    // 視点操作処理
    // マウスや右スティックの入力値を受け取り、カメラの向きを変更する
    void Look(const FInputActionValue& Value);

    // ジャンプ開始処理
    // ジャンプボタンが押されたときに呼ばれる
    void StartJump();

    // ジャンプ終了処理
    // ジャンプボタンが離されたときに呼ばれる
    void StopJump();

protected:
    // ==========================
    // Enhanced Input 関連
    // ==========================

    // プレイヤー用の入力マッピングコンテキスト
    // 各アクション（移動・ジャンプ・視点操作など）を紐づける設定
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputMappingContext* IMC_Player;

    // 移動アクション（WASD入力など）
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Move;

    // ジャンプアクション（スペースキーなど）
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Jump;

    // 視点操作アクション（マウス・右スティック）
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* IA_Look;

    // ==========================
    // カメラ関連コンポーネント
    // ==========================

    // カメラ用のスプリングアーム
    // キャラクターから一定距離を保ってカメラを追従させる
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    // 実際に表示されるカメラ
    // スプリングアームの先端にアタッチされ、プレイヤー視点を制御する
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FollowCamera;
};
