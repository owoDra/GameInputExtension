// Copyright (C) 2024 owoDra

#pragma once

#include "InputProcessor.h"

#include "InputProcessor_MoveAndLook.generated.h"


/**
 * Class for performing specific input processing of actors
 */
UCLASS()
class GEINPUT_API UInputProcessor_MoveAndLook : public UInputProcessor
{
	GENERATED_BODY()
public:
	UInputProcessor_MoveAndLook(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Look")
	float PadLookYawRate{ 300.0f };

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Look")
	float PadLookPitchRate{ 165.0f };

	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> Pawn;

public:
	virtual void OnInitialized_Implementation(UInputProcessComponent* InputComponent) override;
	virtual void OnDeinitialize_Implementation(UInputProcessComponent* InputComponent) override;

protected:
	virtual void OnTriggered_Implementation(const FGameplayTag& InputTag, const FInputActionValue& InputActionValue) override;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookPad(const FInputActionValue& InputActionValue);
};
