
#include "Elevator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MPE/MPECharacter.h"
#include "Blueprint/UserWidget.h"
#include "ElevatorWidget.h"
#include "Shaft/Shaft.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"


AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// Created Meshes
	SceneElevator = CreateDefaultSubobject<USceneComponent>(FName("SceneElevator"));
	RootComponent = SceneElevator;

	Elevator = CreateDefaultSubobject<UStaticMeshComponent>(FName("Elevator"));
	Elevator->SetupAttachment(RootComponent);

	ElevatorDoorRight = CreateDefaultSubobject<UStaticMeshComponent>(FName("DoorRight"));
	ElevatorDoorRight->SetupAttachment(Elevator);
	ElevatorDoorLeft = CreateDefaultSubobject<UStaticMeshComponent>(FName("DoorLeft"));
	ElevatorDoorLeft->SetupAttachment(Elevator);

	/*OuterBox = CreateDefaultSubobject<UBoxComponent>(FName("OuterBox"));
	OuterBox->SetupAttachment(Elevator);*/

	InnerBox = CreateDefaultSubobject<UBoxComponent>(FName("InnerBox"));
	InnerBox->SetupAttachment(Elevator);

	ElevatorPointLight = CreateDefaultSubobject<UPointLightComponent>(FName("Point Light"));
	ElevatorPointLight->AttenuationRadius = 300.0f;
	ElevatorPointLight->SetupAttachment(Elevator);
	//////////////////////////////////////////////////////////////////////////////////////////

	// AudioComponents
	Background_Sound_AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Background_Sound_AudioComp"));
	Background_Sound_AudioComp->SetupAttachment(Elevator);

	Elevator_Arrived_AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Elevator_Arrived_AudioComp"));
	Elevator_Arrived_AudioComp->SetupAttachment(Elevator);

	Elevator_Move_AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Elevator_Move_AudioComp"));
	Elevator_Move_AudioComp->SetupAttachment(Elevator);

	Doors_Opening_Closing_AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("DoorsOpeningClosingAudioComp"));
	Doors_Opening_Closing_AudioComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	TimelinesSetup();
	BindsInBeginPlay();
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
	
	/*OuterBox->SetBoxExtent(OuterBoxExtent);
	OuterBox->SetRelativeLocation(OuterBoxInitLocation);*/

	InnerBox->SetBoxExtent(InnerBoxExtent);
	InnerBox->SetRelativeLocation(InnerBoxInitLocation);
}

void AElevator::TimelinesSetup()
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	if (ElevatorCurveFloat && DoorsCurveFloat)
	{
		FOnTimelineFloat DoorsTimeLineProgress;
		DoorsTimeLineProgress.BindUFunction(this, FName("HandleElevatorDoorsOpenProgress"));
		DoorsCurveTimeLine.AddInterpFloat(DoorsCurveFloat, DoorsTimeLineProgress);

		FOnTimelineFloat ElevatorTimeLineProgress;
		FOnTimelineEventStatic onTimelineFinishedCallback;
		ElevatorTimeLineProgress.BindUFunction(this, FName("HandleElevatorMoveProgress"));
		onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
		ElevatorCurveTimeLine.AddInterpFloat(ElevatorCurveFloat, ElevatorTimeLineProgress);
		ElevatorCurveTimeLine.SetTimelineFinishedFunc(onTimelineFinishedCallback);
	}
}

// Server
void AElevator::BindsInBeginPlay_Implementation()
{
	// Bind Functions On Character Begin Overlap & End Overlap ElevatorBox UBoxComponent
	InnerBox->OnComponentBeginOverlap.AddDynamic(this, &AElevator::OnOverlapBegin);
	InnerBox->OnComponentEndOverlap.AddDynamic(this, &AElevator::OnOverlapEnd);

	// Bind Function On Elevator Outer Panels Hit
	if (!Shafts.IsEmpty())
	{
		for (int32 i = 0; i < Shafts.Num(); i++)
		{
			UKismetSystemLibrary::PrintText(this, INVTEXT("AElevator::BindsInBeginPlay"));
			Shafts[i]->OnOuterPanelHit.AddUniqueDynamic(this, &AElevator::CheckLocation);
		}
	}
}


// ElevatorInnerBox Begin Overlap Callback Functions
// Activates input to open the widget and shows instructions on how to open the widget
void AElevator::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMPECharacter* Character = Cast<AMPECharacter>(OtherActor);

	if (IsValid(Character))
	{
		BackgroundSound(true);
		Character->bIsShow = true;
		Character->ShowHintWidget(true);

		if (IsValid(Character->FloorNumbersWidget))
		{
			Character->FloorNumbersWidget->OnOpenButtonClicked.AddUniqueDynamic(this, &AElevator::CallBack_OpenDoors);
			Character->FloorNumbersWidget->OnFloorButtonClicked.AddUniqueDynamic(this, &AElevator::Server_MoveElevator);
		}
	}
}

// ElevatorInnerBox End Overlap Callback Functions
// Deactivates the input to open the widget and removes instructions on how to open the widget
void AElevator::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMPECharacter* Character = Cast<AMPECharacter>(OtherActor);
	if (IsValid(Character))
	{
		BackgroundSound(false);
		Character->bIsShow = false;
		Character->ShowHintWidget(false);
	}
}

// Moves the elevator using the Timeline
void AElevator::Server_MoveElevator_Implementation(AMPECharacter* Character, int32 TargetFloor)
{
	ClickedFloor = TargetFloor;
	if (CurrentFloorindex == ClickedFloor)
	{
		CallBack_OpenDoors(Character);
		return;
	}

	CallBack_CloseDoors(Character, CurrentFloorindex);
	FVector ActorLocation = Elevator->GetComponentLocation();
	ElevatorCurrentLocation = Elevator->GetComponentLocation();
	
	ActorLocation.Z = FloorHeight * TargetFloor + 0.2f;
	ElevatorTargetLocation = ActorLocation;
	MoveElevator(Character, TargetFloor);
}

void AElevator::MoveElevator_Implementation(AMPECharacter* Character, int32 TargetFloor)
{
	ElevatorCurveTimeLine.Play();
	ElevatorCurveTimeLine.SetNewTime(0);
	PlayElevatorMoveSound();
}

void AElevator::HandleElevatorMoveProgress(float value)
{
	FVector ElevatorNewLocation = FMath::Lerp(ElevatorCurrentLocation, ElevatorTargetLocation, value);
	Elevator->SetWorldLocation(ElevatorNewLocation);
}

void AElevator::Server_OpenDoors_Implementation(AMPECharacter* Character)
{
	DoorsCurveTimeLine.Play();
	if (!Shafts.IsEmpty())
	{
		Shafts[ClickedFloor]->DoorsTimeline.Play();
	}
}

void AElevator::Server_CloseDoors_Implementation()
{
	DoorsCurveTimeLine.Reverse();
	if (!Shafts.IsEmpty())
	{
		Shafts[CurrentFloorindex]->DoorsTimeline.Reverse();
	}
}

void AElevator::HandleElevatorDoorsOpenProgress(float value)
{
	FVector NewRightDoorLocation = FMath::Lerp(RightDoorInitLocation, RightDoorTargetLocation, value);
	FVector NewLeftDoorLocation = FMath::Lerp(LeftDoorInitLocation, LeftDoorTargetLocation, value);
	ElevatorDoorRight->SetRelativeLocation(NewRightDoorLocation);
	ElevatorDoorLeft->SetRelativeLocation(NewLeftDoorLocation);
}

void AElevator::CallBack_OpenDoors(AMPECharacter* Character)
{
	Server_OpenDoors(Character);
	PlayDoorsSound();
}

void AElevator::CallBack_CloseDoors(AMPECharacter* Character, int32 TargetFloor)
{
	Server_CloseDoors();
	PlayDoorsSound();
}

void AElevator::CheckLocation(float ShaftLcationZ, AMPECharacter* BaseCharacter)
{
	FVector ActorLocation = Elevator->GetComponentLocation();
	ClickedFloor = int(ShaftLcationZ / FloorHeight);
	
	//CallBack_CloseDoors(BaseCharacter, CurrentFloorindex);
	Server_MoveElevator(BaseCharacter, ClickedFloor);
}

void AElevator::BackgroundSound(bool bCanPlay)
{
	
	if (IsValid(Background_Sound_AudioComp) && IsValid(Background_SoundBase))
	{
		if (!Background_Sound_AudioComp->IsPlaying() && bCanPlay)
		{
			Background_Sound_AudioComp->SetSound(Background_SoundBase);
			Background_Sound_AudioComp->Play();
		}
		else
		{
			Background_Sound_AudioComp->Stop();
		}
	}
}

void AElevator::PlayElevatorArrivedSound()
{
	if (IsValid(Elevator_Arrived_AudioComp) && IsValid(Elevator_Arrived_SoundBase))
	{
		if (!Elevator_Arrived_AudioComp->IsPlaying())
		{
			Elevator_Arrived_AudioComp->SetSound(Elevator_Arrived_SoundBase);
			Elevator_Arrived_AudioComp->Play();
		}
	}
}

void AElevator::PlayElevatorMoveSound()
{
	if (IsValid(Elevator_Move_AudioComp) && IsValid(Elevator_Move_SoundBase))
	{
		if (!Elevator_Move_AudioComp->IsPlaying())
		{
			Elevator_Move_AudioComp->SetSound(Elevator_Move_SoundBase);
			Elevator_Move_AudioComp->Play();
		}
	}
}

void AElevator::PlayDoorsSound()
{
	if (IsValid(Doors_Opening_Closing_AudioComp) && IsValid(Doors_Opening_Closing_SoundBase))
	{
		if (!Doors_Opening_Closing_AudioComp->IsPlaying())
		{
			Doors_Opening_Closing_AudioComp->SetSound(Doors_Opening_Closing_SoundBase);
			Doors_Opening_Closing_AudioComp->Play();
		}
	}
}

void AElevator::TimelineFinishedCallback()
{
	CurrentFloorindex = ClickedFloor;
	PlayElevatorArrivedSound();

	DoorsCurveTimeLine.Play();
	if (!Shafts.IsEmpty())
	{
		Shafts[ClickedFloor]->DoorsTimeline.Play();
		PlayDoorsSound();
	}

	if (IsValid(Elevator_Move_AudioComp))
	{
		Elevator_Move_AudioComp->Stop();
	}
}
