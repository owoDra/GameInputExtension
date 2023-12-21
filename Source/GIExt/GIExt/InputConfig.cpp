// Copyright (C) 2023 owoDra

#include "InputConfig.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InputConfig)


UInputConfig::UInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UInputConfig::RemoveBinds(UEnhancedInputComponent* InputComponent, TArray<uint32>& BindHandles) const
{
	for (uint32 Handle : BindHandles)
	{
		InputComponent->RemoveBindingByHandle(Handle);
	}

	BindHandles.Reset();
}
