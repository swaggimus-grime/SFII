// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SFII.h"
#include "PaperCharacter.h"
#include "SFIICharacter.generated.h"

//enum used to declare which part of the body each attack hits
UENUM()
enum class AttackPosition : uint8
{
	HIGH				UMETA(DisplayName = "High"),
	MEDIUM				UMETA(DisplayName = "Medium"),
	LOW					UMETA(DisplayName = "Low"),
};

UCLASS(config=Game)
class ASFIICharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	ASFIICharacter();

	//pointer to other player
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Reference")
	ASFIICharacter* OtherPlayer;
	
protected:

	virtual void BeginPlay() override;

	//capsule used for blocking the opponent from walking right through player
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Damage)
	UCapsuleComponent* CollisionCapsule;

	//max health allowed for this character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth = 0;

	//current health of player
	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float CurrentHealth = 0;

	//if player is being hit, set true
	bool bIsHit;

	//delay used for attack lag
	FTimerHandle HitAnimDelay;

	//when health gets depleted
	void OnHealthUpdated();

	//deplete health and update player state
	UFUNCTION(BlueprintCallable, Category = "Stats")
	virtual void UpdateHealth(float DamageTaken, AttackPosition AttackPos);

	//animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* WalkAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpForwardAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* CrouchAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* HighHitAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* KO_Animation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* VictoryAnimation = nullptr;

	//when player is performing actions besides attacks
	void UpdateAnimation();

	//check if desired animation is equal to the current animation being displayed
	UFUNCTION(BlueprintCallable, Category = Animations)
	void CheckNewAnimation(); 

	//walk left or right
	void MoveRight(float Value);

	//crouching
	void OnCrouch();
	void StopCrouching();
	bool bIsCrouching;

	//jumping
	void Jump();

	//attack	
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void Attack(class UPaperFlipbook* AttackAnimation, AttackPosition AttackPos, float AttackDamage);
	void StopAttack();

	//when near opponent
	UFUNCTION()
	void OnOpponentSpaceEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//when away from opponent
	UFUNCTION()
	void OnOpponentSpaceExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//set up inputs
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	//tick
	virtual void Tick(float DeltaSeconds) override;

	//check which side the opponent is on relative to this player
	UFUNCTION(BlueprintPure, Category = "Reference")
	bool IsOtherPlayerRight() const;
	
private:
	//delay handle for lag
	FTimerHandle TimeDelay;

	//animation to be used directly after the current animation
	class UPaperFlipbook* DesiredAnimation; 

	//if player is currently attacking, set true
	bool bIsAttacking;

	//if player is at something that can deal damage, set true
	bool bIsAtDamager;
};