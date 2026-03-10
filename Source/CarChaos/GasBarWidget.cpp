// Fill out your copyright notice in the Description page of Project Settings.


#include "GasBarWidget.h"

void UGasBarWidget::UpdateGasBar()
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
