// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "GasBarWidget.generated.h"

UCLASS()
class CARCHAOS_API UGasBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* GasBar;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
    float MaxGas = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
    float CurrentGas = 100.f;

    UFUNCTION(BlueprintCallable, Category = "Gas")
    void UpdateGasBar();
};
