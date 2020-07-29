// Fill out your copyright notice in the Description page of Project Settings.


#include "SFIIAICharacter.h"
#include "SFIIAIController.h"
#include "SFIICharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

ASFIIAICharacter::ASFIIAICharacter() {
	OpponentSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Opponent Sensor"));
	OpponentSensor->SetPeripheralVisionAngle(90.f);
}

void ASFIIAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (OpponentSensor != nullptr) {
		OpponentSensor->OnSeePawn.AddDynamic(this, &ASFIIAICharacter::OnOpponentVisible);
	}


}

void ASFIIAICharacter::OnOpponentVisible(APawn* Opponent)
{
	AIController = Cast<ASFIIAIController>(GetController());
	if (AIController) {
		//set controller to focus on opponent if opponent is visible
		AIController->SetOpponent(Opponent);
	}
}


		





