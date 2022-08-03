// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrossairWidget.generated.h"


UCLASS()
class MPE_API UCrossairWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void Remove();

protected:

	UPROPERTY()
	TObjectPtr<class AMPECharacter> MyCharacter;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CrossairIMG;
};
