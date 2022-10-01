// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"


// Sets default values
AMainCharacter::AMainCharacter(){

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set Size for Collision Capsule
	GetCapsuleComponent()->SetCapsuleSize(40.f, 90.f);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to much 
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Don't rotate when the controller rotates.
	// Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 530.f; // how high character jumps and how fast his velocity is.
	GetCharacterMovement()->AirControl = 0.2f; // allows to still move the character in space sligthly while in te air.

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;
	bLMBDown = false;

	//Initialize Enums:
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false; 

	bMovingForward = false;
	bMovingRight = false;

	TempAttack = 0;

	HasWeapon = false;
	IsRolling = false;
	bJumping = false;

	AgroTarget = nullptr;
	bHasAgroTarget = false;

	bOnlyWalking = false;
	bFinalBox = false;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay(){

	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController()); // Getting the Controller
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime){

	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) {
		return;
	}

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	if (!bOnlyWalking) {

		switch (StaminaStatus) {

		case EStaminaStatus::ESS_Normal:
			if (bShiftKeyDown && (bMovingForward || bMovingRight)) {

				if (Stamina - DeltaStamina <= MinSprintStamina) {
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else {
					Stamina -= DeltaStamina;
				}

				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else {// Shift key is up

				if (Stamina + DeltaStamina >= MaxStamina) {
					Stamina = MaxStamina;
				}
				else {
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
		case EStaminaStatus::ESS_BelowMinimum:
			if (bShiftKeyDown && (bMovingForward || bMovingRight)) {

				if (Stamina - DeltaStamina <= 0.f) {
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else {
					Stamina -= DeltaStamina;
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
			}
			else { // Shift key is up

				if (Stamina + DeltaStamina >= MinSprintStamina) {
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else {
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
		case EStaminaStatus::ESS_Exhausted:

			if (bShiftKeyDown) {

				Stamina = 0.f;
			}
			else {// Shift key is up

				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;
		case EStaminaStatus::ESS_ExhaustedRecovering:

			if (Stamina + DeltaStamina >= MinSprintStamina) {

				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else {

				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;
		default:
			;
		}
	}

	if (bInterpToEnemy && CombatTarget && !IsRolling) {

		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (AgroTarget) {

		CombatTargetLocation = AgroTarget->GetActorLocation();
		if (MainPlayerController) {

			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

//Find what rotation we need to orient the MainCharacter to the Target
FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target) {

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){

	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); //checks if playerComponent is valid and if not it will stop the code right here

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::ShiftKeyUp);
	
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);
}

void AMainCharacter::MoveForward(float Value){

	bMovingForward = false;
	if ((Controller != nullptr) && (Value != 0.0f) && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead) && !IsRolling) {

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}

void AMainCharacter::MoveRight(float Value){

	bMovingRight = false;
	if ((Controller != nullptr) && (Value != 0.0f) && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead)) {

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true;
	}
}

void AMainCharacter::TurnAtRate(float Rate){

	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate){

	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LMBDown() {

	if ((MovementStatus != EMovementStatus::EMS_Dead) && !IsRolling) {

		bLMBDown = true;

		if (ActiveOverlappingItem) {

			AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);

			if (Weapon) {

				if (Weapon->CoinsAmountToEquip <= Coins) {
					Coins -= Weapon->CoinsAmountToEquip;
					Weapon->Equip(this);
					SetActiveOverlappingItem(nullptr);
					HasWeapon = true;
				}
				else {
					UGameplayStatics::PlaySound2D(this, NotEnoughCoinsSound); //play sound when Not enough coins
				}
			}
		}
		else if (EquippedWeapon) {
			Attack();
			//Stamina -= 40;
		}
	}
}

void AMainCharacter::LMBUp() {

	bLMBDown = false;
}

void AMainCharacter::DecrementHealth(float Amount) {

	if (Health - Amount <= 0.f) {

		Health = 0.f;
		Die(); //player will die.
	}
	else {
		Health -= Amount;
	}
}

void AMainCharacter::IncrementCoins(int32 Amount) {

	Coins += Amount;
}

void AMainCharacter::IncrementHealth(float Amount) {

	if (Health + Amount >= MaxHealth) {
		Health = MaxHealth;
	}
	else {
		Health += Amount;
	}
}

void AMainCharacter::IncrementMaxStamina(float Amount) {

	MaxStamina += Amount;
}

void AMainCharacter::IncrementMaxHealth(float Amount) {

	MaxHealth += Amount;
}

void AMainCharacter::Die() {

	if (MovementStatus == EMovementStatus::EMS_Dead) {
		return;
	}
	
	//Play animation from the Combat Montage:
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage) {

		int32 Section = FMath::RandRange(0,1);

		switch (Section) {
		case 0:
			AnimInstance->Montage_Play(CombatMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Death_1"));
			break;
		case 1:
			AnimInstance->Montage_Play(CombatMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Death_2"));
			break;

		}
	}
	
	SetMovementStatus(EMovementStatus::EMS_Dead);
	if (EquippedWeapon) {
		EquippedWeapon->DeactivateCollision();
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Stop Enemys from continuing to attack the player after Death

	SetCombatTarget(nullptr);
	SetHasCombatTarget(false);
	UpdateCombatTarget(); // Removing Enemie health bar	

}

void AMainCharacter::Jump(){

	if ((MovementStatus != EMovementStatus::EMS_Dead) && (!IsRolling) && (!bAttacking) && !bFinalBox){
		Super::Jump();
	}
}

void AMainCharacter::DeathEnd() {

	//Make sure that after death animation stop for the main character
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

}

void AMainCharacter::SetInterpToEnemy(bool Interp){

	bInterpToEnemy = Interp;
}

void AMainCharacter::SetMovementStatus(EMovementStatus Status) {

	MovementStatus = Status;

if (MovementStatus == EMovementStatus::EMS_Sprinting) {

	GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
}
else {
	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}
}

void AMainCharacter::ShiftKeyDown() {

	bShiftKeyDown = true;
}

void AMainCharacter::ShiftKeyUp() {

	bShiftKeyDown = false;
}

void AMainCharacter::SetEquippedWeapon(AWeapon* WeaponToSet) {

	if (EquippedWeapon) {

		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet;
}

void AMainCharacter::Attack() {

	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead && !bJumping) {

		bAttacking = true;
		SetInterpToEnemy(true);

		//Play animation from the Combat Montage:
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && CombatMontage) {

			int32 Section;
			do {

				Section = FMath::RandRange(0, 4);

			} while (TempAttack == Section);

			TempAttack = Section;

			switch (Section) {

			case 0:
				AnimInstance->Montage_Play(CombatMontage, 1.9f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);

				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.95f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);

				break;
			case 2:
				AnimInstance->Montage_Play(CombatMontage, 1.4f);
				AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);

				break;
			case 3:
				AnimInstance->Montage_Play(CombatMontage, 2.1f);
				AnimInstance->Montage_JumpToSection(FName("Attack_4"), CombatMontage);

				break;
			case 4:
				AnimInstance->Montage_Play(CombatMontage, 2.1f);
				AnimInstance->Montage_JumpToSection(FName("Attack_5"), CombatMontage);

				break;
			default:
				break;
			}
		}
	}
}

void AMainCharacter::AttackEnd() {

	bAttacking = false;
	SetInterpToEnemy(false);

	if (bLMBDown && !IsRolling) {
		Attack();
	}
}

void AMainCharacter::PlaySwingSound() {

	if (EquippedWeapon->SwingSound) {
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {

	DecrementHealth(DamageAmount);

	return DamageAmount;
}

void AMainCharacter::UpdateCombatTarget() {

	TArray<AActor*> OverlappingActors; 
	GetOverlappingActors(OverlappingActors, EnemyFilter); // Get all Enemies that overlap with the MainCharacter

	if (OverlappingActors.Num() == 0) { // if Enemies don't overlap with MainCharacter return
		
		if (MainPlayerController) {
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	if (ClosestEnemy) {

		FVector MainCharacterLocation = GetActorLocation(); // Getting MainCharactor location
		float MinDistance = (ClosestEnemy->GetActorLocation() - MainCharacterLocation).Size();

		for (auto Actor : OverlappingActors) { // Getting MinDistance with Enemies

			AEnemy* Enemy = Cast<AEnemy>(Actor);

			if (Enemy) {
				float DistanceToActor = (Enemy->GetActorLocation() - MainCharacterLocation).Size();

				if (DistanceToActor < MinDistance) {

					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController) {

			MainPlayerController->DisplayEnemyHealthBar();
		}
	}
	SetCombatTarget(ClosestEnemy);
	bHasCombatTarget = true;
}

void AMainCharacter::Visible() {

	GetMesh()->SetVisibility(true);
	EquippedWeapon->SetActorRelativeScale3D(FVector(1.f,1.f,1.f));
}


void AMainCharacter::NotVisible() {

	GetMesh()->SetVisibility(false);
	EquippedWeapon->SetActorRelativeScale3D(FVector(0.f, 0.f, 0.f));
}