// Copyright Epic Games, Inc. All Rights Reserved.

#include "MPECharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController/MPEPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "CrossairWidget.h"
#include "ElevatorWidget.h"
#include "Elevator/Elevator.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// AMPECharacter

AMPECharacter::AMPECharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bReplicates = true;
	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create FirstPerson Camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), "head");
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 0.f)); // Position the camera
	FirstPersonCamera->SetRelativeRotation(FRotator(0.f, 90.f, -90.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Create AudioComponent
	Button_Press = CreateDefaultSubobject<UAudioComponent>(TEXT("Press"));
	Button_Press->SetupAttachment(RootComponent);
}

void AMPECharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(FloorNumbersWidgetSub))
	{
		FloorNumbersWidget = Cast<UElevatorWidget>(CreateWidget(GetWorld(), FloorNumbersWidgetSub));
	}

	if (IsValid(HintSubClass))
	{
		HintWigetRef = Cast<UUserWidget>(CreateWidget(GetWorld(), HintSubClass));
		HintWigetRef->AddToViewport();
		HintWigetRef->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(CameraHintSubClass))
	{
		CameraHintWigetRef = Cast<UUserWidget>(CreateWidget(GetWorld(), CameraHintSubClass));

		CameraHintWigetRef->AddToViewport(0);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
				CameraHintWigetRef->RemoveFromParent();
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			}, 3, false);
	}
}

void AMPECharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPECharacter, bHit);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMPECharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMPECharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMPECharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMPECharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMPECharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMPECharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMPECharacter::TouchStopped);

	// Shows Elevator floors widget Key R 
	PlayerInputComponent->BindAction("ElevatorWidget", IE_Pressed, this, &AMPECharacter::ShowFloorstNumberWidget);
	// LineTrace Key E 
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMPECharacter::InteractPressed);
	// changes the camera from first person to third person and vice versa Key V
	PlayerInputComponent->BindAction("ChangeCamera", IE_Pressed, this, &AMPECharacter::CameraSwitch);
}

void AMPECharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMPECharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMPECharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMPECharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMPECharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMPECharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

// changes the camera from first person to third person and vice versa Key V
void AMPECharacter::CameraSwitch()
{
	if (IsValid(CrossairWidgetSub))
	{
		CrossairWidget = Cast<UCrossairWidget>(CreateWidget(GetWorld(), CrossairWidgetSub));

		if (FollowCamera->IsActive())
		{
			FirstPersonCamera->Activate(true);
			FirstPersonCamera->SetActive(true);
			FollowCamera->Activate(false);
			FollowCamera->SetActive(false);
			bUseControllerRotationYaw = true;

			if (IsValid(CrossairWidget))
			{
				CrossairWidget->AddToViewport(0);
			}
		}
		else
		{
			// Removes CrossairWidget from Viewport in CrosssairWidget.cpp
			if (OnEnterKeyb.IsBound())
			{
				OnEnterKeyb.Broadcast();
			}

			FirstPersonCamera->Activate(false);
			FirstPersonCamera->SetActive(false);
			FollowCamera->Activate(true);
			FollowCamera->SetActive(true);
			bUseControllerRotationYaw = false;
		}
	}
}

// Shows an elevator widget when the player presses the R button
void AMPECharacter::ShowFloorstNumberWidget()
{
	if (IsValid(FloorNumbersWidget) && bIsShow)
	{
		PlayerController = Cast<APlayerController>(GetOwner());

		HintWigetRef->RemoveFromParent();
		if (PlayerController)
		{
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->bShowMouseCursor = true;
		}
		FloorNumbersWidget->AddToViewport(0);
	}
}

// Sets Hint Widget Visibility. Called in Elevator class.
void AMPECharacter::ShowHintWidget(bool bCanShow)
{
	if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
	{
		if (IsValid(HintWigetRef))
		{
			if (bCanShow)
			{
				HintWigetRef->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				HintWigetRef->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}


// LineTrace Key E Server
void AMPECharacter::InteractPressed_Implementation()
{
	FVector Location;
	FRotator Rotation;
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));

	GetController()->GetPlayerViewPoint(Location, Rotation);

	FVector Start = Location;
	FVector End = (Start + (Rotation.Vector() * 300));

	bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, TraceObjectTypes, false, ActorsToIgnore,
		EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Red, FLinearColor::Green, 3.0f);

	if (bHit)
	{
		if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
		{
			UKismetSystemLibrary::PrintText(this, INVTEXT("Interact"));
			PlayButtonSound();
		}

		if (OnOverlaped.IsBound())
		{
			OnOverlaped.Broadcast();
		}
	}
}

void AMPECharacter::PlayButtonSound()
{
	if (IsValid(Button_Press) && IsValid(Button_Press_SoundBase))
	{
		if (!Button_Press->IsPlaying())
		{
			Button_Press->SetSound(Button_Press_SoundBase);
			Button_Press->Play();
		}
	}
}
