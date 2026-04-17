// Fill out your copyright notice in the Description page of Project Settings.


#include "Kismet/GameplayStatics.h"
#include "CarChaosCarPawnPC.h"
#include "RacingCheckpoint.h"
#include "CarChaosRacingGameState.h"

void ACarChaosRacingGameState::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;

	//Set up Checkpoints in Array
	TArray<AActor*> FoundActorsCheckpoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARacingCheckpoint::StaticClass(), FoundActorsCheckpoints);

	for (AActor* Actor : FoundActorsCheckpoints)
	{
		Checkpoints.Add(Cast<ARacingCheckpoint>(Actor));
	}

	Checkpoints.Sort([](const ARacingCheckpoint& A, const ARacingCheckpoint& B)
	{
		return A.CheckpointNumber < B.CheckpointNumber;
	});

	//Set up Cars in Array
	TArray<AActor*> FoundActorsCars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACarChaosCarPawnPC::StaticClass(), FoundActorsCars);

	for (AActor* Actor : FoundActorsCars)
	{
		Cars.Add(Cast<ACarChaosCarPawnPC>(Actor));
	}
}

void ACarChaosRacingGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCarRanking();
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

			if (Checkpoints.IsValidIndex(A.CurrentCheckpoint))
			{
				const FVector NextCheckpointLocation = Checkpoints[A.CurrentCheckpoint]->GetActorLocation();

				const float DistA = FVector::Dist(A.GetActorLocation(), NextCheckpointLocation);
				const float DistB = FVector::Dist(B.GetActorLocation(), NextCheckpointLocation);

				return DistA < DistB; // closer to next checkpoint ranks higher
			}
			else
			{
				//Next checkpoint is finish line so first again
				const FVector NextCheckpointLocation = Checkpoints[0]->GetActorLocation();

				const float DistA = FVector::Dist(A.GetActorLocation(), NextCheckpointLocation);
				const float DistB = FVector::Dist(B.GetActorLocation(), NextCheckpointLocation);

				return DistA < DistB; // closer to next checkpoint ranks higher
			}
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
}

void ACarChaosRacingGameState::LoseRace()
{
	//When player runs out of time or gas
}