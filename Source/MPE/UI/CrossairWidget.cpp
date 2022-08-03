#include "CrossairWidget.h"
#include "MPE/MPECharacter.h"

void UCrossairWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MyCharacter = Cast<AMPECharacter>(GetOwningPlayer()->GetCharacter());

	if (IsValid(MyCharacter))
	{
		MyCharacter->OnEnterKeyb.AddUniqueDynamic(this, &UCrossairWidget::Remove);
	}
}

void UCrossairWidget::Remove()
{
	RemoveFromViewport();
}
