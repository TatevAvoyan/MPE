
#include "Shaft.h"
#include "MPE/MPECharacter.h"
#include "ElevatorWidget.h"
#include "Components/BoxComponent.h"


AShaft::AShaft()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Shaft = CreateDefaultSubobject<UStaticMeshComponent>(FName("Elevator"));
	RootComponent = Shaft;

	ShaftDoorRight = CreateDefaultSubobject<UStaticMeshComponent>(FName("DoorRight"));
	ShaftDoorRight->SetupAttachment(RootComponent);

	ShaftDoorLeft = CreateDefaultSubobject<UStaticMeshComponent>(FName("DoorLeft"));
	ShaftDoorLeft->SetupAttachment(RootComponent);

	OuterPannel = CreateDefaultSubobject<UStaticMeshComponent>(FName("Button"));
	OuterPannel->SetupAttachment(RootComponent);
	OuterPannel->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);

	ShaftInnerBox = CreateDefaultSubobject<UBoxComponent>(FName("Collision Mesh"));
	ShaftInnerBox->SetupAttachment(RootComponent);
	
}

void AShaft::BeginPlay()
{
	Super::BeginPlay();

	ShaftInnerBox->OnComponentBeginOverlap.AddDynamic(this, &AShaft::OnComponentBeginOverlap);
	ShaftInnerBox->OnComponentEndOverlap.AddDynamic(this, &AShaft::OnComponentEndOverlap);

	if (DoorsCurve)
	{
		FOnTimelineFloat TimeLineProgress{};
		TimeLineProgress.BindUFunction(this, FName("HandleProgress"));

		DoorsTimeline.AddInterpFloat(DoorsCurve, TimeLineProgress);
	}
}

void AShaft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoorsTimeline.TickTimeline(DeltaTime);
}

void AShaft::OnConstruction(const FTransform& Transform)
{
	// Set in Class Defaults: Category = "Components Location"
	ShaftDoorRight->SetRelativeLocation(ShaftRightDoorInitLocation);
	
	ShaftDoorLeft->SetRelativeLocation(ShaftLeftDoorInitLocation);
	ShaftDoorLeft->SetRelativeRotation(ShaftLeftDoorInitRotation);
	
	OuterPannel->SetRelativeLocation(ShaftButtonInitLocation);
	
	ShaftInnerBox->SetBoxExtent(ShaftBoxExtent);
	ShaftInnerBox->SetRelativeLocation(ShaftBoxInitLocation);
}

void AShaft::OpenDoors_Implementation(AMPECharacter* BaseCharacter)
{
	CuurentIndex = BaseCharacter->FloorNumbersWidget->ButtonNumber;

	ShaftArr[CuurentIndex]->DoorsTimeline.Play();
}

void AShaft::CloseDoors_Implementation()
{
	ShaftArr[CuurentIndex]->DoorsTimeline.Reverse();
}

void AShaft::HandleProgress_Implementation(float value)
{
	FVector NewShaftRightDoorLocation = FMath::Lerp(ShaftRightDoorInitLocation, ShaftRightDoorTargetLocation, value);
	FVector NewShaftLeftDoorLocation = FMath::Lerp(ShaftLeftDoorInitLocation, ShaftLeftDoorTargetLocation, value);
	ShaftDoorRight->SetRelativeLocation(NewShaftRightDoorLocation);
	ShaftDoorLeft->SetRelativeLocation(NewShaftLeftDoorLocation);
}

void AShaft::OnComponentBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CharacterBase = Cast<AMPECharacter>(OtherActor);
	if (IsValid(CharacterBase))
	{
		CharacterBase->OnOverlaped.AddUniqueDynamic(this, &AShaft::OpenDoors);
		if (IsValid(CharacterBase->FloorNumbersWidget))
		{
			CharacterBase->FloorNumbersWidget->OnOpenButtonClicked.AddUniqueDynamic(this, &AShaft::OpenDoors);
		}
	}
}

void AShaft::OnComponentEndOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ShaftArr[CuurentIndex]->DoorsTimeline.Reverse();
}
