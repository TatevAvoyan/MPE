#include "GetButton.h"

void UGetButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (IsValid(this))
	{
		OnClicked.AddUniqueDynamic(this, &UGetButton::OnButtonPressedEvent);
	}
}

void UGetButton::OnButtonPressedEvent()
{
	if (IsValid(this) && ButtonPresssedEventReturnButton.IsBound())
	{
		ButtonPresssedEventReturnButton.Broadcast(this);
	}
}
