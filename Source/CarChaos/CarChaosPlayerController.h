// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.h"
#include "CarChaosCarPawn.h"
#include "CarChaosPlayerController.generated.h"

UCLASS()
class CARCHAOS_API ACarChaosPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    virtual void Tick(float DeltaTime) override;

    virtual void SetupInputComponent() override;

    void HandleInput(const FInputActionValue& Value);

    void AddGas();

protected:

    virtual void BeginPlay() override;

    void UpdateGasBarVisuals();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Drive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

private:

    UPROPERTY()
    ACarChaosCarPawn* PlayerCarPawn;

    UPROPERTY()
    UMainHUDWidget* MainHUDWidget;

};
