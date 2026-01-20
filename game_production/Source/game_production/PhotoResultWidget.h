// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PhotoResultWidget.generated.h"

UCLASS()
class GAME_PRODUCTION_API UPhotoResultWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ResultText;

    UFUNCTION(BlueprintCallable)
    void SetResultText(const FString& Text);

    UFUNCTION(BlueprintCallable)
    void PlayFadeOut();

protected:
    // UMGで作ったアニメーションをバインド
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeOut;
};
