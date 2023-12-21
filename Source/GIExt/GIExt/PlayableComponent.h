// Copyright (C) 2023 owoDra

#pragma once

#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"

#include "InputActionValue.h"

#include "PlayableComponent.generated.h"

class UInputConfig;


/**
 * Component that initializes the player-controlled Pawn Input
 */
UCLASS(Meta=(BlueprintSpawnableComponent))
class GIEXT_API UPlayableComponent 
	: public UPawnComponent
	, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UPlayableComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//
	// Function name used to add this component
	//
	static const FName NAME_ActorFeatureName;

	static const FName NAME_BindInputsNow;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void InitializePlayerInput();

public:
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;


protected:
	//
	// Current Input Config
	//
	UPROPERTY(EditAnywhere)
	TObjectPtr<const UInputConfig> DefaultInputConfig{ nullptr };

public:
	/**
	 * Set the current Input Config
	 */
	UFUNCTION(BlueprintCallable)
	void SetInputConfig(const UInputConfig* NewInputConfig);


public:
	void AddAdditionalInputConfig(const UInputConfig* InputConfig);
	void RemoveAdditionalInputConfig(const UInputConfig* InputConfig);

protected:
	void TagInput_Pressed(FGameplayTag InputTag);
	void TagInput_Released(FGameplayTag InputTag);

	virtual void TagInput_PressedExtra(FGameplayTag InputTag);
	virtual void TagInput_ReleasedExtra(FGameplayTag InputTag);

	void NativeInput_Move(const FInputActionValue& InputActionValue);
	void NativeInput_LookMouse(const FInputActionValue& InputActionValue);
	void NativeInput_LookStick(const FInputActionValue& InputActionValue);


public:
	UFUNCTION(BlueprintPure, Category = "Component")
	static UPlayableComponent* FindPlayableComponent(const APawn* Pawn);

};
