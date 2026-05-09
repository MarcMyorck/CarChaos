// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUDWidget.h"
#include "CarChaosCarPawnPC.h"
#include <Kismet/GameplayStatics.h>

void UMainHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ResultBG->SetVisibility(ESlateVisibility::Collapsed);
    ResultTextBlock->SetVisibility(ESlateVisibility::Collapsed);
    ResultContinueTextBlock->SetVisibility(ESlateVisibility::Collapsed);

    PointsBonus1TextBlock->SetText(FText::GetEmpty());
    PointsBonus2TextBlock->SetText(FText::GetEmpty());
    PointsBonus3TextBlock->SetText(FText::GetEmpty());
}

void UMainHUDWidget::UpdateGasBar()
{
    if (!GasBar) return;

    CurrentGas = FMath::Clamp(CurrentGas, 0.f, MaxGas);

    float GasPercent = CurrentGas / MaxGas;
    GasBar->SetPercent(GasPercent);

    FLinearColor FullGasColor = FLinearColor::Green;
    FLinearColor LowGasColor = FLinearColor::Red;

    FLinearColor CurrentColor = FLinearColor::LerpUsingHSV(LowGasColor, FullGasColor, GasPercent);
    GasBar->SetFillColorAndOpacity(CurrentColor);
}

void UMainHUDWidget::UpdateTimeLimit()
{
    if (!TimeTextBlock) return;

    int32 TotalSeconds = FMath::FloorToInt(TimeRemaining);
    int32 Minutes = TotalSeconds / 60;
    int32 Seconds = TotalSeconds % 60;
    int32 Milliseconds = FMath::FloorToInt((TimeRemaining - TotalSeconds) * 100);

    FString TimeString = FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, Milliseconds);
    FText TimeText = FText::FromString(TimeString);

    TimeTextBlock->SetText(TimeText);
}

void UMainHUDWidget::UpdatePoints()
{
    if (!PointsTextBlock) return;

    int32 PointsInt = FMath::FloorToInt(CurrentPoints);

    FString PointsString = FString::Printf(TEXT("Score: %d"), PointsInt);
    FText PointsText = FText::FromString(PointsString);

    PointsTextBlock->SetText(PointsText);
}

void UMainHUDWidget::UpdatePointsBonusPosition(float PositionPoints)
{
    if (!PointsBonusPositionTextBlock) return;

    int32 PointsInt = FMath::FloorToInt((PositionPoints * (4.f / (float)CurrentPosition)));

    FString PointsString = FString::Printf(TEXT("+%d/s Position Bonus"), PointsInt);
    FText PointsText = FText::FromString(PointsString);

    PointsBonusPositionTextBlock->SetText(PointsText);
}

void UMainHUDWidget::AddPointsBonus(float Points, FString BonusText)
{
    if (!PointsBonus1TextBlock || !PointsBonus2TextBlock || !PointsBonus3TextBlock) return;

    PointsBonus3Timer = PointsBonus2Timer;
    PointsBonus3TextBlock->SetText(PointsBonus2TextBlock->GetText());

    PointsBonus2Timer = PointsBonus1Timer;
    PointsBonus2TextBlock->SetText(PointsBonus1TextBlock->GetText());

    PointsBonus1Timer = 0.1f;
    int32 PointsInt = FMath::FloorToInt(Points);

    FString PointsString = FString::Printf(TEXT("+%d %s"), PointsInt, *BonusText);
    FText PointsText = FText::FromString(PointsString);

    PointsBonus1TextBlock->SetText(PointsText);
}

void UMainHUDWidget::UpdatePointsBonusTimers(float Time)
{
    if (!PointsBonus1TextBlock || !PointsBonus2TextBlock || !PointsBonus3TextBlock) return;

    if (PointsBonus1Timer > 0.f) {
        PointsBonus1Timer += Time;
        if (PointsBonus1Timer >= PointsBonusDuration) {
            PointsBonus1TextBlock->SetText(FText::GetEmpty());
        }
    }

    if (PointsBonus2Timer > 0.f) {
        PointsBonus2Timer += Time;
        if (PointsBonus2Timer >= PointsBonusDuration) {
            PointsBonus2TextBlock->SetText(FText::GetEmpty());
        }
    }

    if (PointsBonus3Timer > 0.f) {
        PointsBonus3Timer += Time;
        if (PointsBonus3Timer >= PointsBonusDuration) {
            PointsBonus3TextBlock->SetText(FText::GetEmpty());
        }
    }
}

void UMainHUDWidget::UpdatePosition()
{
    if (!PositionImage) return;
    if (CurrentPosition < 1 || CurrentPosition > 4) return;

    PositionImage->SetColorAndOpacity(PositionColors[CurrentPosition - 1]);

    FString PositionString = FString::Printf(TEXT("%d"), CurrentPosition);
    FText PositionText = FText::FromString(PositionString);

    PositionTextBlock->SetText(PositionText);
}

void UMainHUDWidget::UpdateLap()
{
    if (!LapTextBlock) return;

    FString LapString = FString::Printf(TEXT("Lap: %d/3"), CurrentLap);
    FText LapText = FText::FromString(LapString);

    LapTextBlock->SetText(LapText);
}

void UMainHUDWidget::DisplayResult(bool IsWin)
{
    ResultBG->SetVisibility(ESlateVisibility::Visible);
    ResultTextBlock->SetVisibility(ESlateVisibility::Visible);
    ResultContinueTextBlock->SetVisibility(ESlateVisibility::Visible);

    if (IsWin)
    {
        FString ResultString = FString::Printf(TEXT("Finish!"));
        FText ResultText = FText::FromString(ResultString);
        ResultTextBlock->SetText(ResultText);
    }
    else
    {
        FString ResultString = FString::Printf(TEXT("You did not make it!"));
        FText ResultText = FText::FromString(ResultString);
        ResultTextBlock->SetText(ResultText);
    }
}

void UMainHUDWidget::UpdateMinimap()
{
    if (CarRefs.Num() == 0)
    {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACarChaosCarPawnPC::StaticClass(), FoundActors);

        for (AActor* Car : FoundActors)
        {
            ACarChaosCarPawnPC* TempCar = Cast<ACarChaosCarPawnPC>(Car);
            CarRefs.Add(TempCar);
        }
    }

    int NumberNPCSDone = 0;
    for (ACarChaosCarPawnPC* CarRef : CarRefs)
    {
        FVector2D TargetLocation = MapWorldToMinimap(CarRef->CarBodyMesh->GetComponentLocation());

        if (CarRef->IsPlayer)
        {
            MinimapPlayerImage->SetRenderTranslation(TargetLocation);
        }
        else 
        {
            if (NumberNPCSDone == 0)
            {
                MinimapEnemyImage1->SetRenderTranslation(TargetLocation);
                NumberNPCSDone++;
            }
            else if (NumberNPCSDone == 1)
            {
                MinimapEnemyImage2->SetRenderTranslation(TargetLocation);
                NumberNPCSDone++;
            }
            else if (NumberNPCSDone == 2)
            {
                MinimapEnemyImage3->SetRenderTranslation(TargetLocation);
            }
        }
    }
}

FVector2D UMainHUDWidget::MapWorldToMinimap(FVector WorldLocation)
{
    const float MinX = FMath::Min(10730.f, -3200.f);
    const float MaxX = FMath::Max(10730.f, -3200.f);
    const float MinY = FMath::Min(19250.f, -1400.f);
    const float MaxY = FMath::Max(19250.f, -1400.f);

    const float Width = MaxX - MinX;
    const float Height = MaxY - MinY;

    float NormX = (WorldLocation.X - MinX) / Width;
    float NormY = (WorldLocation.Y - MinY) / Height;

    NormY = 1.f - NormY;

    const FVector2D MinimapSize(454.8f, 312.f);

    FVector2D LocalPos;
    LocalPos.X = NormY * MinimapSize.X;
    LocalPos.Y = NormX * MinimapSize.Y;

    FVector2D ScreenPos = FVector2D(20.f, 748.f) + LocalPos;

    return ScreenPos;
}