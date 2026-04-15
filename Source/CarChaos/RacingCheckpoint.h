// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CarChaosCarPawnPC.h"
#include "RacingCheckpoint.generated.h"

UCLASS()
class CARCHAOS_API ARacingCheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ARacingCheckpoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Number")
	int CheckpointNumber = -1;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Collision")
	UBoxComponent* Collision;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACarChaosCarPawnPC> PlayerClass;

	UPROPERTY(EditAnywhere) //Change when enemy exists
	TSubclassOf<ACarChaosCarPawnPC> EnemyClass;

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:	
	virtual void Tick(float DeltaTime) override;

};
