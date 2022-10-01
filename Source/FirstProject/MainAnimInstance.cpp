// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"

void UMainAnimInstance::NativeInitializeAnimation() {

	if (pawn == nullptr) {
		pawn = TryGetPawnOwner();

		if (pawn) {
			MainCharacter = Cast< AMainCharacter>(pawn);
		}
	}
}

void UMainAnimInstance::UpdateAnimationProperties() {

	if (pawn == nullptr) {
		pawn = TryGetPawnOwner();
	}

	if (pawn) {

		FVector speed = pawn->GetVelocity();
		FVector LateralSpeed = FVector(speed.X, speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = pawn->GetMovementComponent()->IsFalling();

		if (MainCharacter == nullptr) {
			MainCharacter = Cast< AMainCharacter>(pawn);
		}
	}

}