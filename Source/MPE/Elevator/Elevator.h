#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MPE/Interface/MyInterface.h"
#include "Elevator.generated.h"

UCLASS()
class MPE_API AElevator : public AActor, public IMyInterface
{
	GENERATED_BODY()
	
public:	
	AElevator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when an instance of this class is placed (in editor) or spawned
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	UFUNCTION()
	void TimelinesSetup();

	UFUNCTION(Server, Reliable)
	void BindsInBeginPlay();

protected:
	// ElevatorInnerBox Begin/End Overlap Callback Functions
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*// ElevatorOuterBox Begin/End Overlap Callback Functions
	UFUNCTION(NetMulticast, Reliable)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/

protected:
	// Elevator move functions & Variables
	UFUNCTION(Server, Reliable)
	void Server_MoveElevator(class AMPECharacter* Character, int32 TargetFloor);
	
	UFUNCTION(NetMulticast, Reliable)
	void MoveElevator(class AMPECharacter* Character, int32 TargetFloor);

	UFUNCTION()
	void HandleElevatorMoveProgress(float value);

	UFUNCTION()
	void TimelineFinishedCallback();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
	FTimeline ElevatorCurveTimeLine;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Timeline")
	TObjectPtr<class UCurveFloat> ElevatorCurveFloat;

protected:
	// Elevator Doors Open/Close functions & Variables
	UFUNCTION(NetMulticast, Reliable)
	void Server_OpenDoors(class AMPECharacter* Character);

	UFUNCTION(NetMulticast, Reliable)
	void Server_CloseDoors();

	UFUNCTION()
	void HandleElevatorDoorsOpenProgress(float value);

public:
	UFUNCTION()
	void CallBack_OpenDoors(class AMPECharacter* Character);

	UFUNCTION()
	void CallBack_CloseDoors(AMPECharacter* Character, int32 TargetFloor);

	UPROPERTY()
	FTimeline DoorsCurveTimeLine;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Timeline")
	TObjectPtr<class UCurveFloat> DoorsCurveFloat;

protected:
	// Elevator Anim Target/Init Locations
	UPROPERTY()
	FVector ElevatorCurrentLocation;

	UPROPERTY()
	FVector ElevatorTargetLocation;

	UPROPERTY()
	float ElevatorLocationZ;

	// Doors Start
	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector RightDoorTargetLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector LeftDoorTargetLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector RightDoorInitLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector LeftDoorInitLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FRotator LeftDoorInitRotation;
	// Doors End

	// Collision Boxes
	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector InnerBoxInitLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector InnerBoxExtent;
	
	UPROPERTY()
	int32 CurrentFloorindex = 0;

protected:
	// Meshes
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Elevator;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> ElevatorDoorRight;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> ElevatorDoorLeft;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UPointLightComponent> ElevatorPointLight;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UBoxComponent> InnerBox;

	/*UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UBoxComponent> OuterBox;*/

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class USceneComponent> SceneElevator = nullptr;

public:
	// Shaft Vars
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shaft, meta = (ClampMin = 2, ClampMax = 8))
	int ShaftFloorsCount;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = Shaft)
	TArray<class AShaft*> Shafts;

	UPROPERTY()
	int32 ClickedFloor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shaft)
	float FloorHeight = 300.f;

	UFUNCTION()
	void CheckLocation(float ShaftLcationZ, AMPECharacter* BaseCharacter);

protected:
	// Sounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	TObjectPtr<class USoundBase> Background_SoundBase;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UAudioComponent> Background_Sound_AudioComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	TObjectPtr<class USoundBase> Elevator_Arrived_SoundBase;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UAudioComponent> Elevator_Arrived_AudioComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	TObjectPtr<class USoundBase> Elevator_Move_SoundBase;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UAudioComponent> Elevator_Move_AudioComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	TObjectPtr<class USoundBase> Doors_Opening_Closing_SoundBase;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UAudioComponent> Doors_Opening_Closing_AudioComp;

private:
	// Sounds Functions
	UFUNCTION()
	void BackgroundSound(bool bCanPlay);

	UFUNCTION()
	void PlayElevatorMoveSound();

	UFUNCTION()
	void PlayElevatorArrivedSound();

	UFUNCTION()
	void PlayDoorsSound();
};
