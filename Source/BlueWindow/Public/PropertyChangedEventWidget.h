// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PropertyChangedEventWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnyPropertyChangedDelegate);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class BLUEWINDOW_API UPropertyChangedEventWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	//virtual void PostDuplicate(bool bDuplicateForPIE) override;
	//virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	//virtual void PostEditImport() override;
	virtual void PostEditUndo() override;
	virtual void PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation) override;
	virtual void PostInterpChange(UProperty* PropertyThatChanged) override;
	//virtual void PostInitProperties() override;

	virtual void OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs) override;

	virtual void SynchronizeProperties() override;

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FAnyPropertyChangedDelegate OnAnyPropertyChanged;

	void NotifyOnAnyPropertyChanged();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AnyPropertyChanged"), Category = "BlueWindow")
		void ReceiveOnAnyPropertyChanged();
};
