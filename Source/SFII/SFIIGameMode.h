// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SFII.h"
#include "GameFramework/GameModeBase.h"
#include "SFIIGameMode.generated.h"

/**
 * The GameMode defines the game being played. It governs the game rules, scoring, what actors
 * are allowed to exist in this game type, and who may enter the game.
 *
 * This game mode just sets the default pawn to be the MyCharacter asset, which is a subclass of SFIICharacter
 */

UCLASS(minimalapi)
class ASFIIGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASFIIGameMode();
    virtual void BeginPlay() override;

    
protected:
    virtual void Tick(float DeltaTime) override;

private:
    
};


