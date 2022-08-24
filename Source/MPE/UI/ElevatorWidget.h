// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Button/GetButton.h"
#include "ElevatorWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloorButtonClicked, int32, ClickedButtonNumber);

class UButton;
class AShaft;

UCLASS()
class MPE_API UElevatorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGetButton> Button_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGetButton> Button_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGetButton> Button_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGetButton> Button_4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGetButton> Button_5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGetButton> Button_6;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGetButton> Button_7;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGetButton> Button_8;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> OpenShaftDoorsButton;

public:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UFUNCTION()
	void Split_Conv_String(const class UButton* CurrentButton);

	UFUNCTION(Client, Unreliable)
	void Client_OpenDoorsOpenButtonClicked();

	UFUNCTION(NetMulticast, Unreliable)
	void Server_ButtonClicked(const class UButton* CurrentButton);

	UFUNCTION(NetMulticast, Unreliable)
	void Server_OpenDoorsOpenButtonClicked();

	UFUNCTION()
	void Client_DeactivateMouse_RemoveWidget();

	UFUNCTION()
	void Client_HintWidget_AddToViewport();

	UPROPERTY(BlueprintAssignable)
	FOnOpenClicked OnOpenButtonClicked;

	UPROPERTY(BlueprintAssignable)
	FOnFloorButtonClicked OnFloorButtonClicked;

	UPROPERTY()
	int32 ButtonNumber = 0;

private:
	UPROPERTY()
	TObjectPtr<class AMPECharacter> MyCharacter;
};
