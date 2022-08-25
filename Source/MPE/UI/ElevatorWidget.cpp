#include "ElevatorWidget.h"
#include "Components/Button.h"
#include "Gameframework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "MPE/MPECharacter.h"
#include "Shaft.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UElevatorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MyCharacter = Cast<AMPECharacter>(GetOwningPlayer()->GetPawn());
	PlayerController = Cast<APlayerController>(GetOwningPlayer());

	if (IsValid(OpenShaftDoorsButton))
	{
		OpenShaftDoorsButton->OnClicked.AddUniqueDynamic(this, &UElevatorWidget::OpenDoorsOpenButtonClicked);
	}

	if (IsValid(Button_1))
	{
		Button_1->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::ButtonClicked);
	}

	if (IsValid(Button_2))
	{
		Button_2->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::ButtonClicked);
	}

	if (IsValid(Button_3))
	{
		Button_3->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::ButtonClicked);
	}

	if (IsValid(Button_4))
	{
		Button_4->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::ButtonClicked);
	}

	if (IsValid(Button_5))
	{
		Button_5->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::ButtonClicked);
	}

	if (IsValid(Button_6))
	{
		Button_6->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::ButtonClicked);
	}

	if (IsValid(Button_7))
	{
		Button_7->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::ButtonClicked);
	}

	if (IsValid(Button_8))
	{
		Button_8->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::ButtonClicked);
	}
}

void UElevatorWidget::ButtonClicked(const UButton* CurrentButton)
{
	UKismetSystemLibrary::PrintString(this, "ButtonClicked");
	Split_Conv_String(CurrentButton);

	DeactivateMouse_RemoveWidget();

	if (IsValid(MyCharacter))
	{
		MyCharacter->PlayButtonSound();
	}

	HintWidget_AddToViewport();

	if (OnFloorButtonClicked.IsBound())
	{
		OnFloorButtonClicked.Broadcast(--ButtonNumber);
	}
}

void UElevatorWidget::OpenDoorsOpenButtonClicked()
{
	UKismetSystemLibrary::PrintString(this, "OpenDoorsOpenButtonClicked");
	if (OnOpenButtonClicked.IsBound())
	{
		OnOpenButtonClicked.Broadcast();
	}

	DeactivateMouse_RemoveWidget();

	if (IsValid(MyCharacter))
	{
		MyCharacter->PlayButtonSound();
	}

	HintWidget_AddToViewport();
}

void UElevatorWidget::Split_Conv_String(const UButton* CurrentButton)
{
	// Splitting String and Converting String to int
	FString Left, Right;
	CurrentButton->GetName().Split("_", &Left, &Right);
	ButtonNumber = UKismetStringLibrary::Conv_StringToInt(Right);
}

void UElevatorWidget::DeactivateMouse_RemoveWidget()
{
	// DeActivated Mouse Cursor
	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;
	}

	// Removing From Viewport when Clicked Button
	RemoveFromViewport();
}

void UElevatorWidget::HintWidget_AddToViewport()
{
	// Adding Hint Widget to Viewport
	if (IsValid(MyCharacter))
	{
		MyCharacter->HintWigetRef->AddToViewport();
	}
}
