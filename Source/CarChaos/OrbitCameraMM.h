// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitCameraMM.generated.h"

UCLASS()
class CARCHAOS_API AOrbitCameraMM : public AActor
{
	GENERATED_BODY()
	
public:	
	AOrbitCameraMM();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitRadius = 300.f;     

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitHeight = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitSpeed = 30.f;       

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float CurrentAngle = 0.f;      

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	AActor* TargetActor;           


protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
