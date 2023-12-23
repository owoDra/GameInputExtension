// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"

#include "InputConfig.generated.h"

class UInputAction;
class UInputMappingContext;


/**
 * Non-mutable data asset that contains input configuration properties.
 */
UCLASS(BlueprintType, Const)
class GIEXT_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
#if WITH_EDITOR
	void PrintBindSuccessLog() const;
#endif

public:
	//
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (ForceInlineRow, Categories = "Input"))
	TMap<FGameplayTag, TObjectPtr<const UInputAction>> NativeInputActions;

	//
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (ForceInlineRow, Categories = "Input"))
	TMap<FGameplayTag, TObjectPtr<const UInputAction>> TagInputActions;

	//
	// InputMappingContext applied by default
	// 
	// Tips:
	//	No need to set if setting by GameFeatureAction, etc.
	//
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> DefaultMappingContext{ nullptr };

public:
	void RemoveBinds(UEnhancedInputComponent* InputComponent, TArray<uint32>& BindHandles) const;

	template<class UserClass, typename FuncType>
	void BindNativeAction(UEnhancedInputComponent* InputComponent, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func) const
	{
		check(InputComponent);

		if (auto IA{ NativeInputActions.FindRef(InputTag) })
		{
#if WITH_EDITOR
			PrintBindSuccessLog();
#endif
			InputComponent->BindAction(IA, TriggerEvent, Object, Func);
		}
	}

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindTagActions(UEnhancedInputComponent* InputComponent, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles) const
	{
		check(InputComponent);

		for (const auto& KVP : TagInputActions)
		{
			const auto& InputAction{ KVP.Value };
			const auto& InputTag(KVP.Key);

			if (InputAction && InputTag.IsValid())
			{
#if WITH_EDITOR
				PrintBindSuccessLog();
#endif

				if (PressedFunc)
				{
					BindHandles.Add(InputComponent->BindAction(InputAction, ETriggerEvent::Started, Object, PressedFunc, InputTag).GetHandle());
				}

				if (ReleasedFunc)
				{
					BindHandles.Add(InputComponent->BindAction(InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputTag).GetHandle());
				}
			}
		}
	}
};
