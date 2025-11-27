// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoResultWidget.h"
#include "Components/TextBlock.h"

void UPhotoResultWidget::SetResultText(const FString& Text)
{
    if (ResultText)
    {
        ResultText->SetText(FText::FromString(Text));
    }
}

void UPhotoResultWidget::PlayFadeOut()
{
    if (FadeOut)
    {
        PlayAnimation(FadeOut);
    }
}