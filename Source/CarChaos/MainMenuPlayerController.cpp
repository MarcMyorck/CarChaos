// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"

void AMainMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (MainMenuWidgetClass)
    {
        MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        if (MainMenuWidgetInstance)
        {
            MainMenuWidgetInstance->AddToViewport();

            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(MainMenuWidgetInstance->TakeWidget());
            SetInputMode(InputMode);
            bShowMouseCursor = true;
        }
    }
}