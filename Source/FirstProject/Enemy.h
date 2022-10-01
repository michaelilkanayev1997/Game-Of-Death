// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"


UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8 {

	EMS_Idle         UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking    UMETA(DisplayName = "Attacking"),
	EMS_Dead         UMETA(DisplayName = "Dead"),

	EMS_MAX          UMETA(DisplayName = "Default_MAX")
};

UCLASS()
class FIRSTPROJECT_API AEnemy : public ACharacter{

	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }

	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue* SwingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* CombatLeftCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	UBoxComponent* CombatRightCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	FTimerHandle AttackTimer;

	UFUNCTION(BlueprintCallable)
	void TeleportEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 TempAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HealthBarYSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMainCharacter* Target);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool BossEnemy1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool BossEnemy2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BossAttack_Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Attack1_Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Attack2_Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Attack3_Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Attack4_Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Attack5_Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Attack6_Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "AI")
	bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingAgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	AMainCharacter* CombatTarget;

	UFUNCTION()
	void LeftCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void LeftCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void RightCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RightCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	void Attack();

	AMainCharacter* TeleportTarget;

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Die(AActor* Causer);

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void VisibilityFalse();

	UFUNCTION(BlueprintCallable)
	void VisibilityTrue();

	UFUNCTION(BlueprintCallable)
	void ChangeEnemyLocation();

	bool Alive();

	void Disappear();

	// -----Interp rotation -------
	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Target);
	FRotator GetLookAtRotationYaw(FVector Target);
	//-----------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool IsDead;
};

