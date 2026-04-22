// Fill out your copyright notice in the Description page of Project Settings.


#include "Kismet/GameplayStatics.h"
#include "CarChaosCarPawnPC.h"
#include "RacingCheckpoint.h"
#include "CarChaosPlayerController.h"
#include "MainHUDWidget.h"
#include "CarChaosRacingGameState.h"

ACarChaosRacingGameState::ACarChaosRacingGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACarChaosRacingGameState::BeginPlay()
{
	Super::BeginPlay();

	//Set up Checkpoints in Array
	TArray<AActor*> FoundActorsCheckpoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARacingCheckpoint::StaticClass(), FoundActorsCheckpoints);

	for (AActor* Checkpoint : FoundActorsCheckpoints)
	{
		Checkpoints.Add(Cast<ARacingCheckpoint>(Checkpoint));
	}

	//Set up Splines in Array
	TArray<AActor*> FoundActorsSplines;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActorsSplines);

	for (AActor* Actor : FoundActorsSplines)
	{
		if (Actor->GetActorLabel().Contains("BP_RacingSpline"))
		{
			TArray<USplineComponent*> SplineMeshes;
			Actor->GetComponents<USplineComponent>(SplineMeshes);

			for (USplineComponent* Comp : SplineMeshes)
			{
				RacingSplines.Add(Comp);
			}
		}
	}

	//Set up Cars in Array
	TArray<AActor*> FoundActorsCars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACarChaosCarPawnPC::StaticClass(), FoundActorsCars);

	for (AActor* Car : FoundActorsCars)
	{
		ACarChaosCarPawnPC* TempCar = Cast<ACarChaosCarPawnPC>(Car);
		TempCar->RacingSpline = RacingSplines[FMath::RandRange(0, RacingSplines.Num() - 1)];
		Cars.Add(TempCar);
	}
}

void ACarChaosRacingGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Verbose, TEXT("GameState Tick"));

	if (!IsCheckpointsSorted) 
	{
		SortCheckpoints();
	}

	UpdateCarRanking();
}

void ACarChaosRacingGameState::SortCheckpoints()
{
	Checkpoints.Sort([](const ARacingCheckpoint& A, const ARacingCheckpoint& B)
	{
		return A.CheckpointNumber < B.CheckpointNumber;
	});

	//Check first three to see if it seems to be correctly sorted
	if (Checkpoints[0]->CheckpointNumber == 1 && Checkpoints[1]->CheckpointNumber == 2 && Checkpoints[2]->CheckpointNumber == 3)
	{
		IsCheckpointsSorted = true;

		for (int32 i = 0; i < Cars.Num(); ++i)
		{
			ACarChaosCarPawnPC* Car = Cars[i];
			if (Car)
			{
				Car->NextCheckpointObject = Checkpoints[0];
			}
		}
	}
}

void ACarChaosRacingGameState::UpdateCarRanking() 
{
	Cars.Sort([&](const ACarChaosCarPawnPC& A, const ACarChaosCarPawnPC& B)
		{
			if (A.RoundsDone != B.RoundsDone)
			{
				return A.RoundsDone > B.RoundsDone; // higher rounds first
			}

			if (A.CurrentCheckpoint != B.CurrentCheckpoint)
			{
				return A.CurrentCheckpoint > B.CurrentCheckpoint; // further checkpoint first
			}

			//Check which car is closer to next checkpoint
			UBoxComponent* CheckpointCollision;

			if (Checkpoints.IsValidIndex(A.CurrentCheckpoint))
			{
				CheckpointCollision = Checkpoints[A.CurrentCheckpoint]->Collision;
			}
			else
			{
				//Next checkpoint is finish line so first again
				CheckpointCollision = Checkpoints[0]->Collision;
			}

			FVector ClosestA;
			CheckpointCollision->GetClosestPointOnCollision(A.CarBodyMesh->GetComponentLocation(), ClosestA);

			FVector ClosestB;
			CheckpointCollision->GetClosestPointOnCollision(B.CarBodyMesh->GetComponentLocation(), ClosestB);

			const float DistA = FVector::Dist(A.CarBodyMesh->GetComponentLocation(), ClosestA);
			const float DistB = FVector::Dist(B.CarBodyMesh->GetComponentLocation(), ClosestB);

			return DistA < DistB; // closer to next checkpoint ranks higher
		});

	for (int32 i = 0; i < Cars.Num(); ++i)
	{
		ACarChaosCarPawnPC* Car = Cars[i];
		if (Car)
		{
			Car->CurrentPosition = i + 1;
		}
	}
}

void ACarChaosRacingGameState::FinishRace()
{
	//When player finishes all rounds successfully
	ACarChaosPlayerController* PC = Cast<ACarChaosPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->MainHUDWidget->DisplayResult(true);
	}
}

void ACarChaosRacingGameState::LoseRace()
{
	//When player runs out of time or gas
	ACarChaosPlayerController* PC = Cast<ACarChaosPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->MainHUDWidget->DisplayResult(false);
	}
}