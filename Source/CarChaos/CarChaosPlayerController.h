// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.h"
#include "CarChaosPlayerController.generated.h"

UCLASS()
class CARCHAOS_API ACarChaosPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
    float MaxGas = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
    float CurrentGas = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
    float GasUsage = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
    float GasPickupValue = 30.f;

    virtual void Tick(float DeltaTime) override;

    void AddGas();

protected:

    virtual void BeginPlay() override;

    void UpdateGasBar();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

private:

    UPROPERTY()
    UMainHUDWidget* MainHUDWidget;
};
