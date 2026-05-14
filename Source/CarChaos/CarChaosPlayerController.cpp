// Fill out your copyright notice in the Description page of Project Settings.


#include "CarChaosPlayerController.h"
#include "CarChaosRacingGameState.h"
#include <Kismet/GameplayStatics.h>

void ACarChaosPlayerController::BeginPlay()
{
    Super::BeginPlay();
    PrimaryActorTick.bCanEverTick = true;

    FInputModeGameOnly InputModeGame;
    SetInputMode(InputModeGame);
    bShowMouseCursor = false;

    //GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACarChaosPlayerController::SetUpPlayerController);
    FTimerHandle TH;
    GetWorld()->GetTimerManager().SetTimer(TH, this, &ACarChaosPlayerController::SetUpPlayerController, 0.5f, false);
}

void ACarChaosPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PlayerCarPawn) return;

    if (PlayerCarPawn->IsInputEnabled)
    {
        UpdateGasBarVisuals();
        UpdateTimeLimitVisuals();
        UpdatePointsVisuals();
        UpdatePositionVisuals();
        UpdateLapVisuals();
    }

    MainHUDWidget->UpdatePointsBonusTimers(DeltaTime);
    MainHUDWidget->UpdateMinimap();
}

void ACarChaosPlayerController::SetUpPlayerController()
{
    MainHUDWidget = CreateWidget<UMainHUDWidget>(this, MainHUDWidgetClass);
    MainHUDWidget->AddToViewport();

    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    int32 PlayerIndex = LocalPlayer->GetLocalPlayerIndex();
    UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    ACarChaosRacingGameState* GS = GetWorld()->GetGameState<ACarChaosRacingGameState>();
    for (ACarChaosCarPawnPC* Car : GS->Cars)
    {
        if (Car && Car->PlayerSlotIndex == PlayerIndex)
        {
            Possess(Car);

            Car->IsPlayer = true;
            Car->PlayerIndex = PlayerIndex;
            Car->MainHUDWidget = MainHUDWidget;
            PlayerCarPawn = Car;
        }
    }

    Subsystem->ClearAllMappings();
    EnableInput(this);
    if (PlayerIndex == 0)
    {
        Subsystem->AddMappingContext(InputMappingContextP1, 0);
    }
    else if (PlayerIndex == 1)
    {
        Subsystem->AddMappingContext(InputMappingContextP2, 0);
    }

    SetupInputComponent();

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

    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    int32 PlayerIndex = LocalPlayer->GetLocalPlayerIndex();
    if (PlayerIndex == 0)
    {
        EIC->BindAction(IA_Drive, ETriggerEvent::Triggered, this, &ACarChaosPlayerController::HandleDriveInput);
        EIC->BindAction(IA_DropOil, ETriggerEvent::Triggered, this, &ACarChaosPlayerController::HandleOilInput);
        EIC->BindAction(IA_DropRocket, ETriggerEvent::Triggered, this, &ACarChaosPlayerController::HandleRocketInput);
        EIC->BindAction(IA_Continue, ETriggerEvent::Triggered, this, &ACarChaosPlayerController::HandleContinueInput);
    }
    else
    {
        EIC->BindAction(IA_Drive1, ETriggerEvent::Triggered, this, &ACarChaosPlayerController::HandleDriveInput);
        EIC->BindAction(IA_DropOil1, ETriggerEvent::Triggered, this, &ACarChaosPlayerController::HandleOilInput);
        EIC->BindAction(IA_DropRocket1, ETriggerEvent::Triggered, this, &ACarChaosPlayerController::HandleRocketInput);
        EIC->BindAction(IA_Continue1, ETriggerEvent::Triggered, this, &ACarChaosPlayerController::HandleContinueInput);
    }
}

void ACarChaosPlayerController::HandleDriveInput(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();

    PlayerCarPawn->ChangeSpeed(Axis.X);
    PlayerCarPawn->Steer(Axis.Y);
}

void ACarChaosPlayerController::HandleOilInput(const FInputActionValue& Value)
{
    const bool Input = Value.Get<bool>();

    if (Input)
    {
        PlayerCarPawn->DropOil();
    }
}

void ACarChaosPlayerController::HandleRocketInput(const FInputActionValue& Value)
{
    const bool Input = Value.Get<bool>();

    if (Input)
    {
        PlayerCarPawn->DropRocket();
    }
}

void ACarChaosPlayerController::HandleContinueInput(const FInputActionValue& Value)
{
    const bool Input = Value.Get<bool>();

    if (Input && PlayerCarPawn->IsRaceFinished)
    {
        UGameplayStatics::OpenLevel(this, FName("MainMenu"));
    }
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
        MainHUDWidget->UpdatePointsBonusPosition(PlayerCarPawn->PointsPerSecond);
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
