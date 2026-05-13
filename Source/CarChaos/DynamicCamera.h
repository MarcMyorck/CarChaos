// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "CarChaosCarPawnPC.h"
#include "DynamicCamera.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARCHAOS_API UDynamicCamera : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDynamicCamera();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
	float MinFOV = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
	float MaxFOV = 105.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
	float FOVInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Distance")
	float MinArmLength = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Distance")
	float MaxArmLength = 450.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Distance")
	float ArmInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|LookAhead")
	float LookAheadDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SideLean")
	float MaxSideLean = 7.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SideLean")
	float SideLeanInterpSpeed = 7.5f;

private:
	ACarChaosCarPawnPC* OwnerPawn;
};
