// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUDWidget.h"

void UMainHUDWidget::UpdateGasBar()
{
    if (!GasBar) return;

    CurrentGas = FMath::Clamp(CurrentGas, 0.f, MaxGas);

    float GasPercent = CurrentGas / MaxGas;
    GasBar->SetPercent(GasPercent);

    FLinearColor FullGasColor = FLinearColor::Green;
    FLinearColor LowGasColor = FLinearColor::Red;

    FLinearColor CurrentColor = FLinearColor::LerpUsingHSV(LowGasColor, FullGasColor, GasPercent);
    GasBar->SetFillColorAndOpacity(CurrentColor);
}

void UMainHUDWidget::UpdateTimeLimit()
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

void UMainHUDWidget::UpdatePoints()
{
    if (!PointsTextBlock) return;

    int32 PointsInt = FMath::FloorToInt(CurrentPoints);

    FString PointsString = FString::Printf(TEXT("Score: %d"), PointsInt);
    FText PointsText = FText::FromString(PointsString);

    PointsTextBlock->SetText(PointsText);
}

void UMainHUDWidget::UpdatePosition()
{
    if (!PositionTextBlock) return;

    FString PositionString = FString::Printf(TEXT("Position: %d"), CurrentPosition);
    FText PositionText = FText::FromString(PositionString);

    PositionTextBlock->SetText(PositionText);
}

void UMainHUDWidget::UpdateLap()
{
    if (!LapTextBlock) return;

    FString LapString = FString::Printf(TEXT("Lap: %d/3"), CurrentLap);
    FText LapText = FText::FromString(LapString);

    LapTextBlock->SetText(LapText);
}