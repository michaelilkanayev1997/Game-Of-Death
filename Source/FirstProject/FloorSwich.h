// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwich.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloorSwich : public AActor{

	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwich();

	/** Overlap volume for functionality to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Swich")
	class UBoxComponent* TriggerBox;

	/** The Switch for the character to step on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Swich")
	class UStaticMeshComponent* FloorSwitch;

	/** Door to move when the floor swtich is stepped on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Swich")
	UStaticMeshComponent* Door;

	/** Initial location for the door*/
	UPROPERTY(BlueprintReadWrite, Category = "Floor Swich")
	FVector InitialDoorLocation;

	/** Initial location for the floor switch */
	UPROPERTY(BlueprintReadWrite, Category = "Floor Swich")
	FVector InitialSwitchLocation;

	FTimerHandle SwitchHandle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Floor Swich")
	float SwitchTime;

	bool bCharacterOnSwitch;

	void CloseDoor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	class USoundCue* OverlapSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void RaiseFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateDoorLocation(float Z);

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateFloorSwitchLocation(float Z);

};
