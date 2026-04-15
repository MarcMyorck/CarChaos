// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GasBarWidget.h"
#include "TimeLimitWidget.h"
#include "PointsWidget.h"
#include "MainHUDWidget.generated.h"

UCLASS()
class CARCHAOS_API UMainHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
    UGasBarWidget* GasBarWidget;

    UPROPERTY(meta = (BindWidget))
    UTimeLimitWidget* TimeLimitWidget;

    UPROPERTY(meta = (BindWidget))
    UPointsWidget* PointsWidget;
};
