// Copyright (C) 2024 owoDra

#pragma once

#include "Factories/Factory.h"

#include "InputProcessorBlueprintFactory.generated.h"


UCLASS(HideCategories=Object, MinimalAPI)
class UInputProcessorBlueprintFactory : public UFactory
{
	GENERATED_BODY()
public:
	UInputProcessorBlueprintFactory(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// The type of blueprint that will be created
	//
	UPROPERTY(EditAnywhere, Category = "InputProcessorBlueprintFactory")
	TEnumAsByte<enum EBlueprintType> BlueprintType;

	//
	// The parent class of the created blueprint
	//
	UPROPERTY(EditAnywhere, Category = "InputProcessorBlueprintFactory")
	TSubclassOf<class UInputProcessor> ParentClass;

public:
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
