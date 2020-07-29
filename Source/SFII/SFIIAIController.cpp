 // Fill out your copyright notice in the Description page of Project Settings.


#include "SFIIAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "SFIIAICharacter.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"


ASFIIAIController::ASFIIAIController() {
    //create components
    BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree"));
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));

    //OpponentKey will have the name "Opponent" in editor
    OpponentKey = "Opponent";
}

void ASFIIAIController::OnPossess(APawn* AIPawn)
{
    Super::OnPossess(AIPawn);
    ASFIIAICharacter* AICharacter = Cast<ASFIIAICharacter>(AIPawn);
    if (AICharacter != nullptr) {
        if (UBlackboardData* BlackboardData = AICharacter->BehaviorTree->GetBlackboardAsset()) {
            //prepare blackboard component for usage with the blackboard asset
            BlackboardComp->InitializeBlackboard(*BlackboardData);
        }
        //start the behavior tree
        BehaviorTreeComp->StartTree(*AICharacter->BehaviorTree);
    }
}

void ASFIIAIController::SetOpponent(APawn* EnemyPawn)
{
    //set the opponent key to represent the opponent
    if (BlackboardComp != nullptr) {
        BlackboardComp->SetValueAsObject(OpponentKey, EnemyPawn);
    }
}
