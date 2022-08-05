
#include "Elevator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MPE/MPECharacter.h"
#include "Blueprint/UserWidget.h"
#include "ElevatorWidget.h"


AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	/*RightDoorInitLocation = (FVector(-30.0f, 0.0f, 0.0f));
	LeftDoorInitLocation = (FVector(-30.0f, 63.0f, 0.0f));

	RightDoorTargetLocation = (FVector(-30.0f, -50.0f, 0.0f));
	LeftDoorTargetLocation = (FVector(-30.0f, 113.0f, 0.0f));*/

	// Created Meshes
	SceneElevator = CreateDefaultSubobject<USceneComponent>(FName("SceneElevator"));
	RootComponent = SceneElevator;

	Elevator = CreateDefaultSubobject<UStaticMeshComponent>(FName("Elevator"));
	Elevator->SetupAttachment(RootComponent);


	ElevatorDoorRight = CreateDefaultSubobject<UStaticMeshComponent>(FName("DoorRight"));
	ElevatorDoorRight->SetupAttachment(Elevator);
	ElevatorDoorLeft = CreateDefaultSubobject<UStaticMeshComponent>(FName("DoorLeft"));
	ElevatorDoorLeft->SetupAttachment(Elevator);

	OuterBox = CreateDefaultSubobject<UBoxComponent>(FName("OuterBox"));
	OuterBox->SetupAttachment(Elevator);

	InnerBox = CreateDefaultSubobject<UBoxComponent>(FName("InnerBox"));
	InnerBox->SetupAttachment(Elevator);

	ElevatorPointLight = CreateDefaultSubobject<UPointLightComponent>(FName("Point Light"));
	ElevatorPointLight->AttenuationRadius = 300.0f;
	ElevatorPointLight->SetupAttachment(Elevator);
	//////////////////////////////////////////////////////////////////////////////////////////
}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind Functions On Character Begin Overlap & End Overlap ElevatorBox UBoxComponent
	InnerBox->OnComponentBeginOverlap.AddDynamic(this, &AElevator::OnOverlapBegin);
	InnerBox->OnComponentEndOverlap.AddDynamic(this, &AElevator::OnOverlapEnd); 
	
	// Bind Functions On Character Begin Overlap & End Overlap ElevatorBox UBoxComponent
	OuterBox->OnComponentBeginOverlap.AddDynamic(this, &AElevator::OnBeginOverlap);
	OuterBox->OnComponentEndOverlap.AddDynamic(this, &AElevator::OnEndOverlap);

	/////////////////////////////////////////////////////////////////////////////////////////////
	if (ElevatorCurveFloat && DoorsCurveFloat)
	{
		FOnTimelineFloat DoorsTimeLineProgress;
		DoorsTimeLineProgress.BindUFunction(this, FName("HandleElevatorDoorsOpenProgress"));
		DoorsCurveTimeLine.AddInterpFloat(DoorsCurveFloat, DoorsTimeLineProgress);

		FOnTimelineFloat ElevatorTimeLineProgress;
		ElevatorTimeLineProgress.BindUFunction(this, FName("HandleElevatorMoveProgress"));
		ElevatorCurveTimeLine.AddInterpFloat(ElevatorCurveFloat, ElevatorTimeLineProgress);
	}
}

// Called every frame
void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Timeline Tick
	ElevatorCurveTimeLine.TickTimeline(DeltaTime);
	DoorsCurveTimeLine.TickTimeline(DeltaTime);

}

void AElevator::OnConstruction(const FTransform& Transform)
{
	// Set in Class Defaults: Category = "Components Location"

	ElevatorDoorLeft->SetRelativeLocation(LeftDoorInitLocation);
	ElevatorDoorLeft->SetRelativeRotation(LeftDoorInitRotation);
	
	ElevatorDoorRight->SetRelativeLocation(RightDoorInitLocation);
	
	OuterBox->SetBoxExtent(OuterBoxExtent);
	OuterBox->SetRelativeLocation(OuterBoxInitLocation);

	InnerBox->SetBoxExtent(InnerBoxExtent);
	InnerBox->SetRelativeLocation(InnerBoxInitLocation);
}

// ElevatorOuterBox Begin Overlap Callback Functions
void AElevator::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMPECharacter* CharacterBase = Cast<AMPECharacter>(OtherActor);

	if (IsValid(CharacterBase))
	{
		CharacterBase->OnOverlaped.AddUniqueDynamic(this, &AElevator::Multi_OpenDoors);
		if (IsValid(CharacterBase->FloorNumbersWidget))
		{
			CharacterBase->FloorNumbersWidget->OnOpenButtonClicked.AddUniqueDynamic(this, &AElevator::Multi_OpenDoors);
		}
	}
}

// ElevatorOuterBox End Overlap Callback Functions
void AElevator::OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Multi_CloseDoors();
}

// ElevatorInnerBox Begin Overlap Callback Functions
// Activates input to open the widget and shows instructions on how to open the widget
void AElevator::OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMPECharacter* Character = Cast<AMPECharacter>(OtherActor);

	if (IsValid(Character))
	{
		if (IsValid(Character->HintWigetRef))
		{
			Character->HintWigetRef->AddToViewport();
		}

		if (IsValid(Character->FloorNumbersWidget))
		{
			Character->FloorNumbersWidget->OnFloorButtonClicked.AddUniqueDynamic(this, &AElevator::Server_MoveElevator);
			Character->bCanShow = true;
		}
	}
}

// ElevatorInnerBox End Overlap Callback Functions
// Deactivates the input to open the widget and removes instructions on how to open the widget
void AElevator::OnOverlapEnd_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMPECharacter* Character = Cast<AMPECharacter>(OtherActor);
	if (IsValid(Character))
	{
		Character->bCanShow = false;

		if (IsValid(Character->HintWigetRef))
		{
			Character->HintWigetRef->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

// Moves the elevator using the Timeline
void AElevator::Server_MoveElevator_Implementation(AMPECharacter* Character, int32 TargetFloor)
{
	FVector ActorLocation = Elevator->GetComponentLocation();
	ElevatorCurrentLocation = Elevator->GetComponentLocation();

	
	ActorLocation.Z = 300 * (TargetFloor - CurrentFloor) + 0.2f;

	ElevatorTargetLocation = ActorLocation;

	MoveElevator(Character, TargetFloor);
}

void AElevator::MoveElevator_Implementation(AMPECharacter* Character, int32 TargetFloor)
{
	ElevatorCurveTimeLine.Play();
	ElevatorCurveTimeLine.SetNewTime(0);
}

void AElevator::HandleElevatorMoveProgress_Implementation(float value)
{
	FVector ElevatorNewLocation = FMath::Lerp(ElevatorCurrentLocation, ElevatorTargetLocation, value);
	Elevator->SetWorldLocation(ElevatorNewLocation);
}

void AElevator::Multi_OpenDoors_Implementation(AMPECharacter* Character)
{
	DoorsCurveTimeLine.Play();
}

void AElevator::Multi_CloseDoors_Implementation()
{
	DoorsCurveTimeLine.Reverse();
}

void AElevator::HandleElevatorDoorsOpenProgress_Implementation(float value)
{
	FVector NewRightDoorLocation = FMath::Lerp(RightDoorInitLocation, RightDoorTargetLocation, value);
	FVector NewLeftDoorLocation = FMath::Lerp(LeftDoorInitLocation, LeftDoorTargetLocation, value);
	ElevatorDoorRight->SetRelativeLocation(NewRightDoorLocation);
	ElevatorDoorLeft->SetRelativeLocation(NewLeftDoorLocation);
}

AElevator* AElevator::GetElevatorRef_Implementation()
{
	return this;
}
