// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TimeLimitWidget.generated.h"

UCLASS()
class CARCHAOS_API UTimeLimitWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimeTextBlock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float TimeRemaining = 0.f;

    UFUNCTION(BlueprintCallable, Category = "Time")
    void UpdateTimeLimit();
};
