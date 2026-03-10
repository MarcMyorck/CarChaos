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

protected:

    virtual void BeginPlay() override;

    // Widget class set in editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

private:

    // Actual widget instance
    UPROPERTY()
    UMainHUDWidget* MainHUDWidget;
};
