// Fill out your copyright notice in the Description page of Project Settings.


#include "CarChaosPlayerController.h"

void ACarChaosPlayerController::BeginPlay()
{
    Super::BeginPlay();
    PrimaryActorTick.bCanEverTick = true;

    MainHUDWidget = CreateWidget<UMainHUDWidget>(this, MainHUDWidgetClass);
    MainHUDWidget->AddToViewport();

    PlayerCarPawn = Cast<ACarChaosCarPawnPC>(GetPawn());
    PlayerCarPawn->IsPlayer = true;
    UpdateGasBarVisuals();
    UpdateTimeLimitVisuals();
    UpdatePointsVisuals();
    UpdatePositionVisuals();
    UpdateLapVisuals();

    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    Subsystem->AddMappingContext(InputMappingContext, 0);
}

void ACarChaosPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateGasBarVisuals();
    UpdateTimeLimitVisuals();
    UpdatePointsVisuals();
    UpdatePositionVisuals();
    UpdateLapVisuals();
}

void ACarChaosPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
    EIC->BindAction(IA_Drive, ETriggerEvent::Triggered, this, &ACarChaosPlayerController::HandleInput);
}

void ACarChaosPlayerController::HandleInput(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();

    PlayerCarPawn->ChangeSpeed(Axis.X);
    PlayerCarPawn->Steer(Axis.Y);
}

void ACarChaosPlayerController::AddGas()
{
    PlayerCarPawn->AddGas();
    UpdateGasBarVisuals();
}

void ACarChaosPlayerController::UpdateGasBarVisuals()
{
    if (MainHUDWidget)
    {
        MainHUDWidget->CurrentGas = PlayerCarPawn->CurrentGas;
        MainHUDWidget->MaxGas = PlayerCarPawn->MaxGas;
        MainHUDWidget->UpdateGasBar();
    }
}

void ACarChaosPlayerController::UpdateTimeLimitVisuals()
{
    if (MainHUDWidget)
    {
        MainHUDWidget->TimeRemaining = PlayerCarPawn->TimeRemaining;
        MainHUDWidget->UpdateTimeLimit();
    }
}

void ACarChaosPlayerController::UpdatePointsVisuals()
{
    if (MainHUDWidget)
    {
        MainHUDWidget->CurrentPoints = PlayerCarPawn->CurrentPoints;
        MainHUDWidget->UpdatePoints();
    }
}

void ACarChaosPlayerController::UpdatePositionVisuals()
{
    if (MainHUDWidget)
    {
        MainHUDWidget->CurrentPosition = PlayerCarPawn->CurrentPosition;
        MainHUDWidget->UpdatePosition();
    }
}

void ACarChaosPlayerController::UpdateLapVisuals()
{
    if (MainHUDWidget)
    {
        MainHUDWidget->CurrentLap = PlayerCarPawn->RoundsDone + 1;
        MainHUDWidget->UpdateLap();
    }
}
