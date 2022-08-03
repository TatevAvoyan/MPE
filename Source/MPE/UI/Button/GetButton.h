// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "GetButton.generated.h"

/**
 * 
 */
UCLASS()
class MPE_API UGetButton : public UButton
{
	GENERATED_BODY()
	
public:
	virtual void SynchronizeProperties() override;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonPresssedEventReturnButton, const class UButton*, CurrentButton);

	UPROPERTY(BlueprintAssignable)
		FOnButtonPresssedEventReturnButton ButtonPresssedEventReturnButton;

	UFUNCTION()
		void OnButtonPressedEvent();
};
