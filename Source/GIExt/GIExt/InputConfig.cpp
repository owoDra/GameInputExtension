// Copyright (C) 2024 owoDra

#include "InputConfig.h"

#if WITH_EDITOR
#include "GIExtLogs.h"
#endif

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


#if WITH_EDITOR
void UInputConfig::PrintBindSuccessLog() const
{
	UE_LOG(LogGIE, Log, TEXT("Bind InputConfig(%s) to action Successed"), *GetNameSafe(this));
}
#endif
