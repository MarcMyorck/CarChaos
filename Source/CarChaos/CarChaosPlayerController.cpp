// Fill out your copyright notice in the Description page of Project Settings.


#include "CarChaosPlayerController.h"

void ACarChaosPlayerController::BeginPlay()
{
    Super::BeginPlay();

    PrimaryActorTick.bCanEverTick = true;

    if (MainHUDWidgetClass)
    {
        MainHUDWidget = CreateWidget<UMainHUDWidget>(this, MainHUDWidgetClass);

        if (MainHUDWidget)
        {
            MainHUDWidget->AddToViewport();
        }
    }

    UpdateGasBar();
}

void ACarChaosPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CurrentGas = FMath::Clamp(CurrentGas - (GasUsage * DeltaTime), 0.f, MaxGas);
    UpdateGasBar();
}

void ACarChaosPlayerController::AddGas()
{
    CurrentGas = FMath::Clamp(CurrentGas + GasPickupValue, 0.f, MaxGas);
    UpdateGasBar();
}

void ACarChaosPlayerController::UpdateGasBar()
{
    if (MainHUDWidget && MainHUDWidget->GasBarWidget)
    {
        MainHUDWidget->GasBarWidget->CurrentGas = CurrentGas;
        MainHUDWidget->GasBarWidget->MaxGas = MaxGas;
        MainHUDWidget->GasBarWidget->UpdateGasBar();
    }
}
