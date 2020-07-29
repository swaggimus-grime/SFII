#include "SFIICharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "PaperFlipbook.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "SFIIAIController.h"
#include "BehaviorTree\BehaviorTreeComponent.h"

//////////////////////////////////////////////////////////////////////////
// ASFIICharacter

ASFIICharacter::ASFIICharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 0.6f;
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->GroundFriction = 10.0f;
	GetCharacterMovement()->MaxWalkSpeed = 100.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision Capsule"));
	CollisionCapsule->SetupAttachment(RootComponent);

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASFIICharacter::OnOpponentSpaceEnter);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ASFIICharacter::OnOpponentSpaceExit);
	bIsCrouching = false;
	bIsAttacking = false;
	bIsAtDamager = false;
	bIsHit = false;

}

//////////////////////////////////////////////////////////////////////////
// Animation

void ASFIICharacter::UpdateHealth(float DamageTaken, AttackPosition AttackPos)
{
	bIsHit = true;
	//deplete health and set hitstun
	if (CurrentHealth > 0.f) {
		CurrentHealth -= DamageTaken;
		if (AttackPos == AttackPosition::HIGH) {
			DesiredAnimation = HighHitAnimation;
			GetWorldTimerManager().SetTimer(HitAnimDelay, this, &ASFIICharacter::OnHealthUpdated, DesiredAnimation->GetTotalDuration());
		}

		if (CurrentHealth < 0.f) {
			CurrentHealth = 0.f;
		}
	}

	//do win pose if health is higher than opponent's at the end of the round
	//else fall down
	if (CurrentHealth == 0.f) {
		DesiredAnimation = KO_Animation;
		GetSprite()->SetLooping(false);
		GetSprite()->SetRelativeLocation(FVector(0.f, 0.f, -15.f));

		if (OtherPlayer->CurrentHealth > 0.f) {
			OtherPlayer->DesiredAnimation = OtherPlayer->VictoryAnimation;
			OtherPlayer->GetSprite()->SetLooping(false);
			OtherPlayer->GetSprite()->SetRelativeLocation(FVector(0.f, 0.f, 15.f));
		}
	}

	CheckNewAnimation();
	OtherPlayer->CheckNewAnimation();
}

void ASFIICharacter::BeginPlay()
{
	Super::BeginPlay();
	//set health to 100%
	CurrentHealth = MaxHealth;
}

void ASFIICharacter::OnHealthUpdated()
{
	bIsHit = false;
	//go back to being idle after hitstun ends
	GetWorldTimerManager().ClearTimer(HitAnimDelay);
	if (CurrentHealth != 0.f) {
		DesiredAnimation = IdleAnimation;
		CheckNewAnimation();
	}
	
}

void ASFIICharacter::UpdateAnimation()
{
	//variables for velocity calculation
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	//if velocity is greater than 0, walk
	//else become idle
	DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? WalkAnimation : IdleAnimation;

	if (GetMovementComponent()) {
		if (!GetMovementComponent()->IsMovingOnGround()) {
			if (PlayerVelocity.X > 0.f)
				//if player has non-zero velocity and is not on ground, jump forward
				DesiredAnimation = JumpForwardAnimation;
			else
				//if player has zero velocity and is not on ground, jump in place
				DesiredAnimation = JumpAnimation;
		}
		else {
			//set animation to crouch 
			if (bIsCrouching)
				DesiredAnimation = CrouchAnimation;
		}

		//check new animation if both players' healths are not 0
		if (CurrentHealth != 0.f && OtherPlayer->CurrentHealth != 0.f) {
			CheckNewAnimation();
		}
	}
}

void ASFIICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//if player is not attacking or being hit, check for other action states
	if (!bIsAttacking && !bIsHit) {
		UpdateAnimation();
	}

	//set rotation of player so that each player faces the other
	if (CurrentHealth != 0) {
		if (IsOtherPlayerRight())
			SetActorRotation(FRotator(0, 0, 0));
		else
			SetActorRotation(FRotator(0, 180, 0));
	}
	
}

bool ASFIICharacter::IsOtherPlayerRight() const {
	//if distance between opponent and this player is positive, set true
	if (IsValid(OtherPlayer)) {
		if (GetActorLocation().X < OtherPlayer->GetActorLocation().X) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

void ASFIICharacter::Attack(UPaperFlipbook* AttackAnimation, AttackPosition AttackPos, float AttackDamage)
{
	if (CurrentHealth > 0.f && OtherPlayer->CurrentHealth > 0.f) {
		//set animation to whatever attack chosen
		bIsAttacking = true;
		DesiredAnimation = AttackAnimation;
		CheckNewAnimation();

		//if player is near something that can deal damage, then health is allowed to be updated
		if (bIsAtDamager) {
			OtherPlayer->UpdateHealth(AttackDamage, AttackPos);
		}
		//activate attack lag 
		DisableInput(Cast<APlayerController>(GetController()));
		GetWorldTimerManager().SetTimer(TimeDelay, this, &ASFIICharacter::StopAttack, AttackAnimation->GetTotalDuration());

	}
}

void ASFIICharacter::StopAttack()
{
	//end attack lag
	bIsAttacking = false;
	GetWorldTimerManager().ClearTimer(TimeDelay);
	EnableInput(Cast<APlayerController>(GetController()));
}

void ASFIICharacter::OnOpponentSpaceEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//when damager is near player, set true
	bIsAtDamager = true;
}

void ASFIICharacter::OnOpponentSpaceExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//when damager is away from player, set false
	bIsAtDamager = false;
}

void ASFIICharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	//set up inputs for user
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASFIICharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASFIICharacter::OnCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASFIICharacter::StopCrouching);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASFIICharacter::MoveRight);

}

void ASFIICharacter::MoveRight(float Value)
{
	
	if (CurrentHealth > 0.f && OtherPlayer->CurrentHealth > 0.f) {
		//if player is not attacking, being hit, or crouching, then player is able to move left or right
		if (!bIsCrouching && !bIsAttacking && !bIsHit) {
			AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
		}
	}

}

void ASFIICharacter::CheckNewAnimation()
{
	//if the current animation is not the same as the desired animation, then set desired animation as current animation
	if (GetSprite()->GetFlipbook() != DesiredAnimation)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void ASFIICharacter::OnCrouch()
{
	//set player to crouch
	bIsCrouching = true;
}

void ASFIICharacter::StopCrouching()
{
	//set player to uncrouch
	bIsCrouching = false;
}

void ASFIICharacter::Jump()
{
	//allow player to jump if both players' healths are not 0
	if (CurrentHealth > 0.f && OtherPlayer->CurrentHealth > 0.f) {
		Super::Jump();
	}
}
