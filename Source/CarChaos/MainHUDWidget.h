// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "MainHUDWidget.generated.h"

UCLASS()
class CARCHAOS_API UMainHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* GasBar;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
    float MaxGas = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gas")
    float CurrentGas = 100.f;

    UFUNCTION(BlueprintCallable, Category = "Gas")
    void UpdateGasBar();

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimeTextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float TimeRemaining = 0.f;

    UFUNCTION(BlueprintCallable, Category = "Time")
    void UpdateTimeLimit();

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PointsTextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Points")
    float CurrentPoints = 0.f;

    UFUNCTION(BlueprintCallable, Category = "Points")
    void UpdatePoints();

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PositionTextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
    int CurrentPosition = 0;

    UFUNCTION(BlueprintCallable, Category = "Position")
    void UpdatePosition();

    UPROPERTY(meta = (BindWidget))
    UTextBlock* LapTextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lap")
    int CurrentLap = 0;

    UFUNCTION(BlueprintCallable, Category = "Lap")
    void UpdateLap();

    UPROPERTY(meta = (BindWidget))
    UImage* ResultBG;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ResultTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ResultContinueTextBlock;

    UFUNCTION(BlueprintCallable, Category = "Result")
    void DisplayResult(bool IsWin);
};
