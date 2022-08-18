#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MPECharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverlaped, AMPECharacter*, MyCharacter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterKeyb);



UCLASS(config=Game)
class AMPECharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;
public:
	AMPECharacter();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Switch camera in V keyb */
	UFUNCTION()
	void CameraSwitch();
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UPROPERTY(BlueprintAssignable)
	FOnOverlaped OnOverlaped;

	UPROPERTY(BlueprintAssignable)
	FOnEnterKeyb OnEnterKeyb;

public:
	UFUNCTION()
	void ShowFloorstNumberWidget();

	UFUNCTION()
	void ShowHintWidget(bool bCanShow);

	UFUNCTION(Server, Reliable)
	void InteractPressed();

	UPROPERTY(Replicated)
	bool bHit;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class APlayerController> PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> FloorNumbersWidgetSub;

	UPROPERTY()
	TObjectPtr<class UElevatorWidget> FloorNumbersWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> CrossairWidgetSub;

	UPROPERTY()
	TObjectPtr<class UCrossairWidget> CrossairWidget;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> HintSubClass;

	UPROPERTY()
	TObjectPtr<class UUserWidget> HintWigetRef;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> CameraHintSubClass;

	UPROPERTY()
	TObjectPtr<class UUserWidget> CameraHintWigetRef;

	UPROPERTY()
	bool bIsShow = false;

	UPROPERTY()
	bool FirstPerson;

protected:
	// Sounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	TObjectPtr<class USoundBase> Button_Press_SoundBase;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UAudioComponent> Button_Press = nullptr;

public:
	UFUNCTION()
	void PlayButtonSound();
};

