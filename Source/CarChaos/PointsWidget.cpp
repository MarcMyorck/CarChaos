// Fill out your copyright notice in the Description page of Project Settings.


#include "PointsWidget.h"

void UPointsWidget::UpdatePoints()
{
    if (!PointsTextBlock) return;

    int32 PointsInt = FMath::FloorToInt(CurrentPoints);

    FString PointsString = FString::Printf(TEXT("Score: %d"), PointsInt);
    FText PointsText = FText::FromString(PointsString);

    PointsTextBlock->SetText(PointsText);
}
