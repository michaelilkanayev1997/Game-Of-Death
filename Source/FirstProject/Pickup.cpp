// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

APickup::APickup() {


}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor) {

		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter) {

			if (OverlapParticles) {

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			OnPickupBP(MainCharacter); // Provide implementaion in the Blueprint
			MainCharacter->PickupLocations.Add(GetActorLocation()); //Adding the location of the pickUp
			if (OverlapSound) {

				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			//Destroy(); //Destroy the actor and everthing inside the actor when Overlap.
		}
	}
}

void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}