// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SFII.h"
#include "SFIICharacter.h"
#include "SFIIAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class SFII_API ASFIIAICharacter : public ASFIICharacter
{
	GENERATED_BODY()

public:

	ASFIIAICharacter();

	//getter for AI controller
	UFUNCTION()
	FORCEINLINE class ASFIIAIController* GetAIController() const { return AIController; };

public:
	//behavior tree asset to be used for the AI player
	UPROPERTY(EditDefaultsOnly, Category = AI)
	class UBehaviorTree* BehaviorTree;

protected:
	virtual void BeginPlay() override;

private:
	//when opponent is visible to AI
	UFUNCTION()
	void OnOpponentVisible(APawn* Opponent);

private:
	//vision of AI
	UPROPERTY(VisibleAnywhere, Category = AI)
	class UPawnSensingComponent* OpponentSensor;

	//pointer to AI controller
	class ASFIIAIController* AIController;
	
};
