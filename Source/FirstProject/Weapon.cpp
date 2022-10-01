// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/AudioComponent.h"
#include "Engine/Texture2D.h"

AWeapon::AWeapon() {

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	bWeaponParticles = false;

	WeaponState = EWeaponState::EWS_Pickup;

	Damage = 25.f;
	CoinsAmountToEquip = 0.f;
}

void AWeapon::BeginPlay(){

	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	// CombatCollision Parameters:
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	if (GeneralWeaponSound) {
		AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, GeneralWeaponSound, GetActorLocation()); //play General Weapon sound 
	}
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor) {

		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter) {

			MainCharacter->SetActiveOverlappingItem(this);
		}
	}

}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){

	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor) {

		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter) {

			MainCharacter->SetActiveOverlappingItem(nullptr);
		}
	}

}

void AWeapon::Equip(AMainCharacter* Char ){

	if (Char) {

		SetInstigator(Char->GetController());

		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);
	
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket) {


			RightHandSocket->AttachActor(this, Char->GetMesh()); //Attaching the weapon to the socket
			bRotate = false; // when weapon equipped stop rotation.
			Char->SetEquippedWeapon(this);
			WeaponState = EWeaponState::EWS_Equipped;
			Char->SetActiveOverlappingItem(nullptr);

			
		}
		if (OnEquipSpecialSound) {

			UGameplayStatics::PlaySound2D(this, OnEquipSpecialSound); //play Equip Special Sound 
		}
		if (OnEquipSound) {

			UGameplayStatics::PlaySound2D(this, OnEquipSound); //play sound when equpping the weapon
		}
		if (!bWeaponParticles) {

			IdleParticleComponent->Deactivate();
		}
		if (GeneralWeaponSound) {
			AudioComponent->SetActive(false); //Stop General Weapon Sound
		}
		if (EquipmentParticle) {
			FVector ParticleLocation1 = FVector(-82174.537109, -0.328125,-500.0);
			FVector ParticleLocation2 = FVector(82174.537109, -0.328125, -500.0);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquipmentParticle, ParticleLocation1, FRotator(0.f), true);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquipmentParticle, ParticleLocation2, FRotator(0.f), true);
		}
		ChangeWeaponIcon(); //BlueprintFunction
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

	if (OtherActor) {
		
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		if (Enemy) {

			if (Enemy->HitParticles) {

				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");

				if (WeaponSocket) {

					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), true);
				}
			}
			if (Enemy->HitSound) {

				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass) {
				//Calling the TakeDamage function in Enemy
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}


}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){



}

void AWeapon::ActivateCollision(){

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision(){

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
