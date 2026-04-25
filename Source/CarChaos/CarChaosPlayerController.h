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
#include "CarChaosCarPawnPC.h"
#include "CarChaosPlayerController.generated.h"

UCLASS()
class CARCHAOS_API ACarChaosPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    virtual void Tick(float DeltaTime) override;

    virtual void SetupInputComponent() override;

    void HandleDriveInput(const FInputActionValue& Value);

    void HandleOilInput(const FInputActionValue& Value);

    void HandleContinueInput(const FInputActionValue& Value);

    void AddGas();

    UPROPERTY()
    UMainHUDWidget* MainHUDWidget;

protected:

    virtual void BeginPlay() override;

    void UpdateGasBarVisuals();

    void UpdateTimeLimitVisuals();

    void UpdatePointsVisuals();

    void UpdatePositionVisuals();

    void UpdateLapVisuals();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Drive;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_DropOil;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Continue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

private:

    UPROPERTY()
    ACarChaosCarPawnPC* PlayerCarPawn;

};
