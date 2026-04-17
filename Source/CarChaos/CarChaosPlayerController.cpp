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
    if (MainHUDWidget && MainHUDWidget->GasBarWidget)
    {
        MainHUDWidget->GasBarWidget->CurrentGas = PlayerCarPawn->CurrentGas;
        MainHUDWidget->GasBarWidget->MaxGas = PlayerCarPawn->MaxGas;
        MainHUDWidget->GasBarWidget->UpdateGasBar();
    }
}

void ACarChaosPlayerController::UpdateTimeLimitVisuals()
{
    if (MainHUDWidget && MainHUDWidget->TimeLimitWidget)
    {
        MainHUDWidget->TimeLimitWidget->TimeRemaining = PlayerCarPawn->TimeRemaining;
        MainHUDWidget->TimeLimitWidget->UpdateTimeLimit();
    }
}

void ACarChaosPlayerController::UpdatePointsVisuals()
{
    if (MainHUDWidget && MainHUDWidget->PointsWidget)
    {
        MainHUDWidget->PointsWidget->CurrentPoints = PlayerCarPawn->CurrentPoints;
        MainHUDWidget->PointsWidget->UpdatePoints();
    }
}
