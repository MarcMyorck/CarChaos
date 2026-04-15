// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PointsWidget.generated.h"

UCLASS()
class CARCHAOS_API UPointsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PointsTextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Points")
    float CurrentPoints = 0.f;

    UFUNCTION(BlueprintCallable, Category = "Points")
    void UpdatePoints();
};
