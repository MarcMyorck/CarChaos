// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "MainHUDWidget.generated.h"

class ACarChaosCarPawnPC;

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
    UTextBlock* PointsBonusPositionTextBlock;

    UFUNCTION(BlueprintCallable, Category = "Point Bonus")
    void UpdatePointsBonusPosition(float PositionPoints);

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PointsBonus1TextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PointsBonus2TextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PointsBonus3TextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point Bonus")
    float PointsBonus1Timer = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point Bonus")
    float PointsBonus2Timer = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point Bonus")
    float PointsBonus3Timer = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point Bonus")
    float PointsBonusDuration = 3.f;

    UFUNCTION(BlueprintCallable, Category = "Point Bonus")
    void AddPointsBonus(float Points, FString BonusText);

    UFUNCTION(BlueprintCallable, Category = "Point Bonus")
    void UpdatePointsBonusTimers(float Time);

    UPROPERTY(meta = (BindWidget))
    UImage* PositionImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PositionTextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
    TArray<FLinearColor> PositionColors;

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

    UPROPERTY(meta = (BindWidget))
    UImage* MinimapImage;

    UPROPERTY(meta = (BindWidget))
    UImage* MinimapPlayerImage;

    UPROPERTY(meta = (BindWidget))
    UImage* MinimapEnemyImage1;

    UPROPERTY(meta = (BindWidget))
    UImage* MinimapEnemyImage2;

    UPROPERTY(meta = (BindWidget))
    UImage* MinimapEnemyImage3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    TArray<ACarChaosCarPawnPC*> CarRefs;

    UFUNCTION(BlueprintCallable, Category = "Minimap")
    void UpdateMinimap();

    UFUNCTION(BlueprintCallable, Category = "Minimap")
    FVector2D MapWorldToMinimap(FVector WorldLocation);
};
