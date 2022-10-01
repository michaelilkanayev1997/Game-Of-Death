// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwich.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"



// Sets default values
AFloorSwich::AFloorSwich(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);

	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));

	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());

	SwitchTime = 2.f;
	bCharacterOnSwitch = false;
}



// Called when the game starts or when spawned
void AFloorSwich::BeginPlay(){

	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwich::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwich::OnOverlapEnd);

	InitialDoorLocation = Door->GetComponentLocation();
	InitialSwitchLocation = FloorSwitch->GetComponentLocation();
	
}

// Called every frame
void AFloorSwich::Tick(float DeltaTime){

	Super::Tick(DeltaTime);
}

void AFloorSwich::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin."));

	if (!bCharacterOnSwitch)
		bCharacterOnSwitch = true;

	RaiseDoor();
	LowerFloorSwitch();
}

void AFloorSwich::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){

	UE_LOG(LogTemp, Warning, TEXT("Overlap End."));

	if (bCharacterOnSwitch)
		bCharacterOnSwitch = false;

	GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwich::CloseDoor,SwitchTime);
}

void AFloorSwich::UpdateDoorLocation(float Z) {

	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z += Z;
	Door->SetWorldLocation(NewLocation);

}

void AFloorSwich::UpdateFloorSwitchLocation(float Z) {

	FVector NewLocation = InitialSwitchLocation;
	NewLocation.Z += Z;
	FloorSwitch->SetWorldLocation(NewLocation);
}

void AFloorSwich::CloseDoor() {

	if (!bCharacterOnSwitch) {

		LowerDoor();
		RaiseFloorSwitch();
	}	
}