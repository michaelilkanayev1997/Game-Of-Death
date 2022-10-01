// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8 {

	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8 {

	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_API AMainCharacter : public ACharacter{

	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnlyWalking;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bFinalBox;
	

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 TempAttack;

	TArray<FVector> PickupLocations;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	/** Set Movement status and running speed */
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Running")
	bool bShiftKeyDown;

	/** Pressed down to enable sprinting */
	void ShiftKeyDown();

	/***/
	void ShiftKeyUp();

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rates to scale turning function for the camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/**
	/*
	/*Player Stats
	/* 
	*/
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite ,Category = "Player Stats")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* NotEnoughCoinsSound;

	UFUNCTION(BlueprintCallable)
	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	void Die();

	virtual void Jump() override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called for forwards/backwards input
	void MoveForward(float Value);

	// Called for side to side input
	void MoveRight(float Value);

	bool bMovingForward;
	bool bMovingRight;


	/** Called via input to turn at a given rate
	* @parm Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up/down at a given rate
	* @parm Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	*
	*/
	void LookUpAtRate(float Rate);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bLMBDown;

	void LMBDown();
	void LMBUp();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class Aitem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(Aitem* Item) { ActiveOverlappingItem = Item; }


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	UFUNCTION(BlueprintCallable)
	void Visible();

	UFUNCTION(BlueprintCallable)
	void NotVisible();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool HasWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Roll")
	bool IsRolling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	bool bJumping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* AgroTarget;

	FORCEINLINE void SetAgroTarget(AEnemy* Target) { AgroTarget = Target; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasAgroTarget;

	FORCEINLINE void SetHasAgroTarget(bool HasTarget) { bHasAgroTarget = HasTarget; }

	UFUNCTION(BlueprintCallable)
	void IncrementMaxStamina(float Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementMaxHealth(float Amount);
};
