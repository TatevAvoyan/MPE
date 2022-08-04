#pragma once

#include "CoreMinimal.h"
#include "Elevator.h"
#include "Shaft.generated.h"


UCLASS()
class MPE_API AShaft : public AActor
{
	GENERATED_BODY()

public:
	AShaft();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called Every frame
	virtual void Tick(float DeltaTime) override;

	// Called when an instance of this class is placed (in editor) or spawned
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	// Opens Elevator & shaft doors
	UFUNCTION(NetMulticast, Reliable)
	void OpenDoors(class AMPECharacter* BaseCharacter);

	// Opens Elevator & shaft doors
	UFUNCTION(NetMulticast, Reliable)
	void OutsideOpenDoors(class AMPECharacter* BaseCharacter);

	// Closes Elevator & shaft doors
	UFUNCTION(NetMulticast, Reliable)
	void CloseDoors();

private:
	///////////////////////////////////////////////////////////////////////
	// Collision Box functions

	UFUNCTION(NetMulticast, Reliable)
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	///////////////////////////////////////////////////////////////////////
	// Timeline functions & Variables

	UFUNCTION(NetMulticast, Reliable)
	void HandleProgress(float value);

	UPROPERTY()
	FTimeline DoorsTimeline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timeline")
	TObjectPtr <class UCurveFloat> DoorsCurve;

	UPROPERTY()
	int32 CuurentIndex = 0;

protected:
	// Meshes
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class USceneComponent> SceneShaft = nullptr;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Shaft;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> OuterPannel;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> ShaftDoorRight;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> ShaftDoorLeft;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UPointLightComponent> ShaftPointLight;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<class UBoxComponent> ShaftInnerBox;

protected:
	// Components Location
	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector ShaftRightDoorTargetLocation; // (X=-22.000000,Y=-55.000000,Z=0.000000)

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector ShaftLeftDoorTargetLocation; // (X=-22.000000,Y=55.000000,Z=0.000000)

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector ShaftRightDoorInitLocation; // (X=-22.000000,Y=0.000000,Z=0.000000)

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector ShaftLeftDoorInitLocation; // (X=-22.000000,Y=0.000000,Z=0.000000)

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FRotator ShaftLeftDoorInitRotation; // (Pitch=0.000000,Yaw=180.000000,Roll=0.000000)

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector ShaftButtonInitLocation; // (X=0.000000,Y=-110.000000,Z=80.000000)

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector ShaftBoxExtent; // FVector(50.0f, 130.0f, 100.0f)

	UPROPERTY(EditDefaultsOnly, Category = "Components Location")
	FVector ShaftBoxInitLocation; // FVector(60.f, 0.f, 108.f)

public:
	TObjectPtr<class AMPECharacter> CharacterBase;

	UPROPERTY(BlueprintReadWrite, Category = "ShaftArr")
	TArray<class AShaft*> ShaftArr;

	UPROPERTY(EditDefaultsOnly, Category = "ElevatorSub")
	TSubclassOf<class AElevator> ElevatorSub;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shaft Ref")
	AElevator* ElevatorObj;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shaft Ref")
	TObjectPtr<class AElevator> ElevatorRef;
};
