// Fill out your copyright notice in the Description page of Project Settings.


#include "item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "MainCharacter.h"


// Sets default values
Aitem::Aitem(){

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisonVolume"));
	RootComponent = CollisionVolume;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	IdleParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticleComponent"));
	IdleParticleComponent->SetupAttachment(GetRootComponent());

	bRotate = false;
	RotationRate = 45.f;
}

// Called when the game starts or when spawned
void Aitem::BeginPlay(){

	Super::BeginPlay();
	
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &Aitem::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &Aitem::OnOverlapEnd);
}

// Called every frame
void Aitem::Tick(float DeltaTime){

	Super::Tick(DeltaTime);

	if (bRotate) {

		FRotator Rotation = GetActorRotation();
		Rotation.Yaw += DeltaTime * RotationRate;
		SetActorRotation(Rotation);
	}
}

void Aitem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

	//UE_LOG(LogTemp, Warning, TEXT("Super::On Overlap Begin!"));
}

void Aitem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){

	//UE_LOG(LogTemp, Warning, TEXT("Super::On Overlap End!"));
}

