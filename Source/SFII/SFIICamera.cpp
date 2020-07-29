// Fill out your copyright notice in the Description page of Project Settings.


#include "SFIICamera.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASFIICamera::ASFIICamera()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
		// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (IsValid(ActorToFollow)) {
		CameraX = ActorToFollow->GetActorLocation().X;
		if (FixedZVal != 0) {
			CameraZ = FixedZVal;
		}
		else {
			CameraZ = ActorToFollow->GetActorLocation().Z + DeadZoneZ;
		}
		//SetActorLocation(FVector(CameraX, YOffset, CameraZ));
	}
}

// Called when the game starts or when spawned
void ASFIICamera::BeginPlay()
{
	Super::BeginPlay();
	SetActorRotation(FRotator(0, 0, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ActorToFollow->GetActorLocation()).Yaw));
	
}

float ASFIICamera::GetNewXVal()
{
	if (float MoveLeft = ActorToFollow->GetActorLocation().X - DeadZoneX > GetActorLocation().X) {
		return MoveLeft;
	}
	else {
		if (float MoveRight = ActorToFollow->GetActorLocation().X + DeadZoneX < GetActorLocation().X) {
			return MoveRight;
		}
		else {
			return CameraX;
		}
	}
}

float ASFIICamera::GetNewZVal()
{
	return 0.0f;
}

// Called every frame
void ASFIICamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
//	CameraX = GetNewXVal();
//	SetActorRelativeLocation(FVector(CameraX, YOffset, CameraZ));

}

