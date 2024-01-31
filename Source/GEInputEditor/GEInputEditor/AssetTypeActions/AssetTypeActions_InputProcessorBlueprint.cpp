// Copyright (C) 2024 owoDra

#include "AssetTypeActions_InputProcessorBlueprint.h"

#include "Processor/InputProcessorBlueprint.h"
#include "Processor/InputProcessor.h"
#include "Factory/InputProcessorBlueprintFactory.h"
#include "GEInputEditor.h"

#include "Misc/MessageDialog.h"
#include "BlueprintEditor.h"
#include "Kismet2/BlueprintEditorUtils.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_InputProcessorBlueprint::GetSupportedClass() const
{
	return UInputProcessorBlueprint::StaticClass();
}

FText FAssetTypeActions_InputProcessorBlueprint::GetName() const
{ 
	return LOCTEXT("AssetTypeActions_InputProcessorBlueprint", "Input Processor"); 
}

FColor FAssetTypeActions_InputProcessorBlueprint::GetTypeColor() const
{
	return FColor(153, 204, 255);
}

uint32 FAssetTypeActions_InputProcessorBlueprint::GetCategories()
{
	return FGEInputEditorModule::Category;
}

const TArray<FText>& FAssetTypeActions_InputProcessorBlueprint::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("SubMenus_InputProcessorBlueprint", "Processor")
	};

	return SubMenus;
}


void FAssetTypeActions_InputProcessorBlueprint::OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor )
{
	auto Mode{ EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone };

	for (auto ObjIt{ InObjects.CreateConstIterator() }; ObjIt; ++ObjIt)
	{
		if (auto Blueprint{ Cast<UBlueprint>(*ObjIt) })
		{
			auto bLetOpen{ true };

			if (!Blueprint->ParentClass)
			{
				const auto MessageReplyType
				{
					FMessageDialog::Open(EAppMsgType::YesNo,
						LOCTEXT("FailedToLoadInputProcessorBlueprintWithContinue", "InputProcessor Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed! Do you want to continue (it can crash the editor)?"))
				};

				bLetOpen = (EAppReturnType::Yes == MessageReplyType);
			}
		
			if (bLetOpen)
			{
				TSharedRef<FBlueprintEditor> NewEditor{ new FBlueprintEditor() };

				TArray<UBlueprint*> Blueprints;
				Blueprints.Add(Blueprint);

				NewEditor->InitBlueprintEditor(Mode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(Blueprint));
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadInputProcessorBlueprint", "InputProcessor Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}


UFactory* FAssetTypeActions_InputProcessorBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	auto* InputProcessorBlueprintFactory{ NewObject<UInputProcessorBlueprintFactory>() };

	InputProcessorBlueprintFactory->ParentClass = TSubclassOf<UInputProcessor>(*InBlueprint->GeneratedClass);

	return InputProcessorBlueprintFactory;
}


bool FAssetTypeActions_InputProcessorBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const
{
	return FBlueprintEditorUtils::IsDataOnlyBlueprint(Blueprint)
		&& !FBlueprintEditorUtils::IsLevelScriptBlueprint(Blueprint)
		&& !FBlueprintEditorUtils::IsInterfaceBlueprint(Blueprint)
		&& !Blueprint->bForceFullEditor
		&& !Blueprint->bIsNewlyCreated;
}

#undef LOCTEXT_NAMESPACE
