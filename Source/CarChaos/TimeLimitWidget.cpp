// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeLimitWidget.h"

void UTimeLimitWidget::UpdateTimeLimit()
{
    if (!TimeTextBlock) return;

    int32 TotalSeconds = FMath::FloorToInt(TimeRemaining);
    int32 Minutes = TotalSeconds / 60;
    int32 Seconds = TotalSeconds % 60;
    int32 Milliseconds = FMath::FloorToInt((TimeRemaining - TotalSeconds) * 100);

    FString TimeString = FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, Milliseconds);
    FText TimeText = FText::FromString(TimeString);

    TimeTextBlock->SetText(TimeText);
}
