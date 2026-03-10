// Fill out your copyright notice in the Description page of Project Settings.


#include "CarChaosPlayerController.h"

void ACarChaosPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (MainHUDWidgetClass)
    {
        MainHUDWidget = CreateWidget<UMainHUDWidget>(this, MainHUDWidgetClass);

        if (MainHUDWidget)
        {
            MainHUDWidget->AddToViewport();
        }
    }

    if (MainHUDWidget && MainHUDWidget->GasBarWidget)
    {
        MainHUDWidget->GasBarWidget->CurrentGas = 80.f;
        MainHUDWidget->GasBarWidget->MaxGas = 100.f;
        MainHUDWidget->GasBarWidget->UpdateGasBar();
    }
}
