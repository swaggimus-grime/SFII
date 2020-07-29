// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SFII.h"
#include "AIController.h"
#include "SFIIAIController.generated.h"

/**
 * 
 */
UCLASS()
class SFII_API ASFIIAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	ASFIIAIController();

	//set the opponent of pawn
	void SetOpponent(APawn* EnemyPawn);

	//getters for AI components
	FORCEINLINE class UBehaviorTreeComponent* GetBehaviorTreeComponent() const { return BehaviorTreeComp; }
	FORCEINLINE class UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComp; }
private:

	//when controller possesses pawn
	virtual void OnPossess(APawn* AIPawn) override;

private:
	//AI components
	class UBehaviorTreeComponent* BehaviorTreeComp;
	class UBlackboardComponent* BlackboardComp;

	//key that represents the opponent
	FName OpponentKey;


	
};
