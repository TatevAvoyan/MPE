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
	// ElevatorInnerBox Begin/End Overlap Callback Functions
	UFUNCTION(NetMulticast, Reliable)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// ElevatorOuterBox Begin/End Overlap Callback Functions
	UFUNCTION(NetMulticast, Reliable)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Elevator move functions & Variables
	UFUNCTION(Server, Reliable)
	void Server_MoveElevator(class AMPECharacter* Character, int32 TargetFloor);
	
	UFUNCTION(NetMulticast, Reliable)
	void MoveElevator(class AMPECharacter* Character, int32 TargetFloor);

	UFUNCTION(NetMulticast, Reliable)
	void HandleElevatorMoveProgress(float value);

public:
	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
	FTimeline ElevatorCurveTimeLine;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Timeline")
	TObjectPtr<class UCurveFloat> ElevatorCurveFloat;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	class AElevator* GetElevatorRef();

protected:
	// Elevator Doors Open/Close functions & Variables
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OpenDoors(class AMPECharacter* Character);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_CloseDoors();

	UFUNCTION(NetMulticast, Reliable)
	void HandleElevatorDoorsOpenProgress(float value);

	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
	FTimeline DoorsCurveTimeLine;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Timeline")
	TObjectPtr<class UCurveFloat> DoorsCurveFloat;

protected:
	// Elevator Anim Target/Init Locations

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

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector ElevatorCurrentLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector ElevatorTargetLocation;

	// Boxes Start
	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector OuterBoxInitLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector OuterBoxExtent;

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector InnerBoxInitLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector InnerBoxExtent;
	// Boxes End

	UPROPERTY(EditDefaultsOnly, Category = "Floor", meta = (ClampMin = 1, ClampMax = 8))
	int32 CurrentFloor = 1;

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

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UBoxComponent> OuterBox;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class USceneComponent> SceneElevator = nullptr;

public:
	// Shaft Vars
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floors Cout", meta = (ClampMin = 2, ClampMax = 8))
	int ShaftFloorsCount;
};
