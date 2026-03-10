// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GasBarWidget.h"
#include "MainHUDWidget.generated.h"

UCLASS()
class CARCHAOS_API UMainHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // This connects to the HealthBar widget inside the HUD blueprint
    UPROPERTY(meta = (BindWidget))
    UGasBarWidget* GasBarWidget;
};
