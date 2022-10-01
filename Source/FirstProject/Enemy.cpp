// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AEnemy::AEnemy(){
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore); // Ignore WorldDynamic Objects

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	CombatLeftCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatLeftCollision"));
	CombatLeftCollision->SetupAttachment(GetMesh(), FName("EnemyLeftSocket"));
	
	CombatRightCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatRightCollision"));
	CombatRightCollision->SetupAttachment(GetMesh(), FName("EnemyRightSocket"));

	bOverlappingCombatSphere = false;
	bOverlappingAgroSphere = false;

	Health = 75.f;
	MaxHealth = 100.f;
	Damage = 10.f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 2.0f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	DeathDelay = 5.f; // The time after the death of the enemy that it will take the enemy to disappear
	HealthBarYSize = 70.f;

	BossEnemy1 = false;
	BossEnemy2 = false;

	TeleportTarget = nullptr;

	TempAttack = 1;

	BossAttack_Speed = 1.2f;
	Attack1_Speed = 1.35f;
	Attack2_Speed = 1.2f;
	Attack3_Speed = 1.35f;
	Attack4_Speed = 1.0f;
	Attack5_Speed = 1.0f;
	Attack6_Speed = 1.0f;

	InterpSpeed = 10.f;
	bInterpToEnemy = false;

	IsDead = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay(){
	Super::BeginPlay();
	
	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	CombatLeftCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::LeftCombatOnOverlapBegin);
	CombatLeftCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::LeftCombatOnOverlapEnd);

	CombatRightCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::RightCombatOnOverlapBegin);
	CombatRightCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::RightCombatOnOverlapEnd);

	// CombatCollision Parameters:
	CombatLeftCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatLeftCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatLeftCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatLeftCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatRightCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatRightCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatRightCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatRightCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Setting Camera to ignore Enemy mesh (fixing camera in combat)
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called every frame
void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if(bOverlappingAgroSphere)

	if (bInterpToEnemy && CombatTarget && !bAttacking && bOverlappingCombatSphere) {

		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}
}

FRotator AEnemy::GetLookAtRotationYaw(FVector Target) {

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){

	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

	if (OtherActor && Alive()) {
		
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter) {

			//UE_LOG(LogTemp, Warning, TEXT("Agro Sphere Begin !")); //******** Check **********//
			bOverlappingAgroSphere = true;

			if (!bAttacking) {
				MoveToTarget(MainCharacter);
			}
			MainPlayerController = Cast<AMainPlayerController>(MainCharacter->GetController()); // Getting MainCharacter Controller
			if (MainPlayerController) {
				MainPlayerController->HealthBarY = HealthBarYSize;
			}

			TeleportTarget = MainCharacter;
			CombatTarget = MainCharacter;
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){

	if (OtherActor && OtherComp) {
	
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter) {
			//UE_LOG(LogTemp, Warning, TEXT("Agro Sphere End !")); //******** Check **********//
			if (MainCharacter->CombatTarget == this) {

				MainCharacter->SetCombatTarget(nullptr);
			}

			MainCharacter->SetAgroTarget(nullptr);
			MainCharacter->SetHasAgroTarget(false);

			MainCharacter->SetHasCombatTarget(false);

			MainPlayerController = Cast<AMainPlayerController>(MainCharacter->GetController()); // Getting MainCharacter Controller
			if (MainPlayerController) {
				MainPlayerController->RemoveEnemyHealthBar();
			}
			//MainCharacter->UpdateCombatTarget();  // Removing Enemie health bar
		
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);

			if (AIController) {
				AIController->StopMovement();
			}
		}
		CombatTarget = nullptr;
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

	if (OtherActor && Alive()) {

		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter) {
			//UE_LOG(LogTemp, Warning, TEXT("Combat Sphere Begin !")); //******** Check **********//
			MainCharacter->SetAgroTarget(this);
			MainCharacter->SetHasAgroTarget(true);

			MainCharacter->SetCombatTarget(this);
			MainCharacter->SetHasCombatTarget(true);

			if (MainCharacter->CombatTarget == this) {

				MainCharacter->UpdateCombatTarget(); // Display Enemy Health bar
			}
			//MainCharacter->UpdateCombatTarget();

			bOverlappingCombatSphere = true;

			Attack();
			CombatTarget = MainCharacter; //**For some reason Sometimes when there are several enemies together it resets to nullptr
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){

	if (OtherActor) {
		
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter) {
			//UE_LOG(LogTemp, Warning, TEXT("Combat Sphere End !")); //******** Check **********//

			bOverlappingCombatSphere = false;

			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking) {
				
				MoveToTarget(MainCharacter);

				if (MainCharacter->CombatTarget == this){

					MainCharacter->UpdateCombatTarget();
				}
			}

			MainCharacter->SetCombatTarget(nullptr);
			MainCharacter->SetHasCombatTarget(false);
			//GetWorldTimerManager().ClearTimer(AttackTimer); //clears the AttackTimer
		}
	}
}

void AEnemy::MoveToTarget(AMainCharacter* Target){

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController){

		//UE_LOG(LogTemp, Warning,TEXT("MoveToTarget()")); //******** Check **********//

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target); // Move to main character
		MoveRequest.SetAcceptanceRadius(20.0f);

		FNavPathSharedPtr NavPath; // Where the information will be kept

		AIController->MoveTo(MoveRequest, &NavPath);

		//-------------- when first MoveTo() Fails --------
		FAIMoveRequest MoveRequest2;
		MoveRequest2.SetGoalActor(Target); // Move to main character
		MoveRequest2.SetAcceptanceRadius(20.0f);
		AIController->MoveTo(MoveRequest2, &NavPath);
		// ------------------------------------------------
	}
}

void AEnemy::LeftCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor) {

		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter) {

			if (MainCharacter->HitParticles) {

				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocketLeft");

				if (TipSocket) {

					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainCharacter->HitParticles, SocketLocation, FRotator(0.f), true);
				}
			}
			if (MainCharacter->HitSound) {

				UGameplayStatics::PlaySound2D(this, MainCharacter->HitSound);
			}
			if (DamageTypeClass) {

				//Calling the TakeDamage function in MainCharacter
				UGameplayStatics::ApplyDamage(MainCharacter, Damage, AIController, this, DamageTypeClass); 
			}
		}
	}
}

void AEnemy::LeftCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}

void AEnemy::RightCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor) {

		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter) {

			if (MainCharacter->HitParticles) {

				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocketRight");

				if (TipSocket) {

					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainCharacter->HitParticles, SocketLocation, FRotator(0.f), true);
				}
			}
			if (MainCharacter->HitSound) {

				UGameplayStatics::PlaySound2D(this, MainCharacter->HitSound);
			}
			if (DamageTypeClass) {

				//Calling the TakeDamage function in MainCharacter
				UGameplayStatics::ApplyDamage(MainCharacter, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::RightCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}

void AEnemy::ActivateCollision() {

	if (CombatTarget) {
		if (!(CombatTarget->IsRolling)) {
			CombatLeftCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			CombatRightCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		if (SwingSound) {
			UGameplayStatics::PlaySound2D(this, SwingSound);
		}
	}
}

void AEnemy::DeactivateCollision() {

	CombatLeftCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatRightCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack() {
	
	//UE_LOG(LogTemp, Warning, TEXT("Attack !")); //******** Check **********//

	if (Alive()) {
		if (AIController) {

			AIController->StopMovement(); //Stop movement in order to start attacking
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		if (!bOverlappingCombatSphere && CombatTarget)
		{
			MoveToTarget(CombatTarget);
			return;
		}

		if (!bAttacking) {
			
			bAttacking = true;

			//Play animation from the Combat Montage:
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance && CombatMontage) {

				int32 Section;
				do {

					if (BossEnemy1) {
						Section = FMath::RandRange(0,4);
					}
					else if (BossEnemy2) {
						Section = FMath::RandRange(0,6);
					}
					else {
						Section = FMath::RandRange(0,2);
					}

				} while (TempAttack == Section);

				TempAttack = Section;

				switch (Section) {

				case 0:
					AnimInstance->Montage_Play(CombatMontage, Attack1_Speed);
					AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
					break;
				case 1:
					AnimInstance->Montage_Play(CombatMontage, Attack2_Speed);
					AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
					break;
				case 2:
					AnimInstance->Montage_Play(CombatMontage, Attack3_Speed);
					AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);
					break;
				case 3:
					AnimInstance->Montage_Play(CombatMontage, BossAttack_Speed);
					AnimInstance->Montage_JumpToSection(FName("BossEnemy"), CombatMontage);
					break;
				case 4:
					AnimInstance->Montage_Play(CombatMontage, Attack4_Speed);
					AnimInstance->Montage_JumpToSection(FName("Attack_4"), CombatMontage);
					break;
				case 5:
					AnimInstance->Montage_Play(CombatMontage, Attack5_Speed);
					AnimInstance->Montage_JumpToSection(FName("Attack_5"), CombatMontage);
					break;
				case 6:
					AnimInstance->Montage_Play(CombatMontage, Attack6_Speed);
					AnimInstance->Montage_JumpToSection(FName("Attack_6"), CombatMontage);
					break;
				default:
					break;
				}
			}
		}
	}
}

void AEnemy::TeleportEnd() {

	bAttacking = false;
	MoveToTarget(TeleportTarget);
}

void AEnemy::AttackEnd() {
	SetInterpToEnemy(false);
	bAttacking = false;

	// As long as the MainCharacter is still overlapping with the combat sphere, enemy will continue to attack
	if (bOverlappingCombatSphere) {

		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);//Random time to wait before attacking again
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	
	}
	else {
		if (CombatTarget) {
			MoveToTarget(CombatTarget);
		}
	}
	SetInterpToEnemy(true);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser){

	if (Health - DamageAmount <= 0.f) {

		Health -= DamageAmount;
		Die(DamageCauser);
	}
	else {
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AEnemy::Die(AActor* Causer) {

	GetWorldTimerManager().ClearTimer(AttackTimer); //clears the AttackTimer

	//Play animation from the Combat Montage:
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && CombatMontage) {

		AnimInstance->Montage_Play(CombatMontage, 0.6f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	//Setting all the collision volumes to no Collision after Death:
	CombatLeftCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatRightCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AMainCharacter* MainCharacter = Cast<AMainCharacter>(Causer);
	if (MainCharacter) {

		if (MainCharacter->CombatTarget == this) {

			MainCharacter->UpdateCombatTarget();
		}
	}
}

void AEnemy::DeathEnd() {
	IsDead = true;
	//Make sure that after death animation stop for this enemy
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	GetWorldTimerManager().SetTimer(DeathTimer,this,&AEnemy::Disappear, DeathDelay);
}

void AEnemy::VisibilityTrue() {

	GetMesh()->SetVisibility(true);
}

void AEnemy::VisibilityFalse() {

	GetMesh()->SetVisibility(false);
}

bool AEnemy::Alive() {

	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear() {
	Destroy();
}

void AEnemy::ChangeEnemyLocation() {

	FVector EnemyCurrentLocation = GetActorLocation();

	float Xdistance1 = FMath::RandRange(300.f,700.f);
	float ydistance1 = FMath::RandRange(300.f,700.f);

	float Xdistance2 = FMath::RandRange(-300.f, -700.f);
	float ydistance2 = FMath::RandRange(-300.f, -700.f);

	int32 LocationOption = FMath::RandRange(0,3);

	switch (LocationOption) {

	case 0:
		SetActorLocation(FVector(EnemyCurrentLocation.X + Xdistance1, EnemyCurrentLocation.Y + ydistance1, EnemyCurrentLocation.Z));
		break;
	case 1:
		SetActorLocation(FVector(EnemyCurrentLocation.X + Xdistance1, EnemyCurrentLocation.Y + ydistance2, EnemyCurrentLocation.Z));
		break;
	case 2:
		SetActorLocation(FVector(EnemyCurrentLocation.X + Xdistance2, EnemyCurrentLocation.Y + ydistance1, EnemyCurrentLocation.Z));
		break;
	case 3:
		SetActorLocation(FVector(EnemyCurrentLocation.X + Xdistance2, EnemyCurrentLocation.Y + ydistance2, EnemyCurrentLocation.Z));
		break;
	default:
		break;
	}

	//Find what rotation we need to orient the Enemy to the MainCharacter
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TeleportTarget->GetActorLocation());
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

	SetActorRotation(LookAtRotationYaw);
}

void AEnemy::SetInterpToEnemy(bool Target) {
	bInterpToEnemy = Target;
}