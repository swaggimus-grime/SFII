// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SFII.h"
#include "GameFramework/Actor.h"
#include "SFIICamera.generated.h"

UCLASS()
class SFII_API ASFIICamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASFIICamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	AActor* ActorToFollow;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraX;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraZ;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float DeadZoneX;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float DeadZoneZ;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float YOffset;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float FixedZVal;

	UFUNCTION()
	float GetNewXVal();

	UFUNCTION()
	float GetNewZVal();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
