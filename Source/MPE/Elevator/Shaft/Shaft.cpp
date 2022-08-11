
#include "Shaft.h"
#include "MPE/MPECharacter.h"
#include "ElevatorWidget.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"


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

		this->DoorsTimeline.AddInterpFloat(DoorsCurve, TimeLineProgress);
	}
}

void AShaft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->DoorsTimeline.TickTimeline(DeltaTime);
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

void AShaft::OutsideOpenDoors_Implementation(AMPECharacter* BaseCharacter)
{
	ShaftLocation = GetActorLocation().Z;

	if (OnOuterPanelHit.IsBound())
	{
		OnOuterPanelHit.Broadcast(ShaftLocation, BaseCharacter);
	}
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
		CharacterBase->OnOverlaped.AddUniqueDynamic(this, &AShaft::OutsideOpenDoors);
	}
}

void AShaft::OnComponentEndOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CharacterBase = Cast<AMPECharacter>(OtherActor);
	if (IsValid(CharacterBase))
	{
		CharacterBase->OnOverlaped.RemoveDynamic(this, &AShaft::OutsideOpenDoors);
	}
}
