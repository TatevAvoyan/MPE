#include "ElevatorWidget.h"
#include "Components/Button.h"
#include "Gameframework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "MPE/MPECharacter.h"
#include "Shaft.h"
#include "Kismet/KismetStringLibrary.h"

void UElevatorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MyCharacter = Cast<AMPECharacter>(GetOwningPlayer()->GetCharacter());
	PlayerController = Cast<APlayerController>(GetOwningPlayer());

	if (IsValid(OpenShaftDoorsButton))
	{
		// OpenShaftDoorsButton->OnClicked.AddUniqueDynamic(this, &UElevatorWidget::Client_OpenDoorsOpenButtonClicked);
		OpenShaftDoorsButton->OnClicked.AddUniqueDynamic(this, &UElevatorWidget::Server_OpenDoorsOpenButtonClicked);
	}

	if (IsValid(Button_1))
	{
		Button_1->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Client_ButtonClicked);
		Button_1->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Server_ButtonClicked);
	}

	if (IsValid(Button_2))
	{
		Button_2->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Client_ButtonClicked);
		Button_2->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Server_ButtonClicked);
	}

	if (IsValid(Button_3))
	{
		Button_3->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Client_ButtonClicked);
		Button_3->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Server_ButtonClicked);
	}

	if (IsValid(Button_4))
	{
		Button_4->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Client_ButtonClicked);
		Button_4->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Server_ButtonClicked);
	}

	if (IsValid(Button_5))
	{
		Button_5->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Client_ButtonClicked);
		Button_5->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Server_ButtonClicked);
	}

	if (IsValid(Button_6))
	{
		Button_6->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Client_ButtonClicked);
		Button_6->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Server_ButtonClicked);
	}

	if (IsValid(Button_7))
	{
		Button_7->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Client_ButtonClicked);
		Button_7->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Server_ButtonClicked);
	}

	if (IsValid(Button_8))
	{
		Button_8->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Client_ButtonClicked);
		Button_8->ButtonPresssedEventReturnButton.AddUniqueDynamic(this, &UElevatorWidget::Server_ButtonClicked);
	}
}

void UElevatorWidget::Server_OpenDoorsOpenButtonClicked_Implementation()
{
	if (OnOpenButtonClicked.IsBound())
	{
		OnOpenButtonClicked.Broadcast(MyCharacter);
	}

	Client_OpenDoorsOpenButtonClicked();
}

void UElevatorWidget::Client_OpenDoorsOpenButtonClicked_Implementation()
{
	/*if (OnOpenButtonClicked.IsBound())
	{
		OnOpenButtonClicked.Broadcast(MyCharacter);
	}*/

	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;
	}

	RemoveFromViewport();

	if (IsValid(MyCharacter))
	{
		MyCharacter->HintWigetRef->AddToViewport(0);
	}
}

void UElevatorWidget::Server_ButtonClicked_Implementation(const UButton* CurrentButton)
{
	if (OnFloorButtonClicked.IsBound())
	{
		OnFloorButtonClicked.Broadcast(MyCharacter, ButtonNumber);
	}
}


void UElevatorWidget::Client_ButtonClicked_Implementation(const UButton* CurrentButton)
{
	FString Left, Right;
	CurrentButton->GetName().Split("_", &Left, &Right);
	ButtonNumber = UKismetStringLibrary::Conv_StringToInt(Right);

	/*if (OnFloorButtonClicked.IsBound())
	{
		OnFloorButtonClicked.Broadcast(MyCharacter, ButtonNumber);
	}*/

	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;
	}

	RemoveFromViewport();

	if (IsValid(MyCharacter))
	{
		MyCharacter->HintWigetRef->AddToViewport(0);
	}
}
