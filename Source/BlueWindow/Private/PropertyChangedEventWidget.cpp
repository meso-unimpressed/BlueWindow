// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "PropertyChangedEventWidget.h"
#include "UnrealType.h"

//void UPropertyChangedEventWidget::PostDuplicate(bool bDuplicateForPIE)
//{
//	Super::PostDuplicate(bDuplicateForPIE);
//	NotifyOnAnyPropertyChanged();
//}

//void UPropertyChangedEventWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//	NotifyOnAnyPropertyChanged();
//}

//void UPropertyChangedEventWidget::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeChainProperty(PropertyChangedEvent);
//	NotifyOnAnyPropertyChanged();
//}

//void UPropertyChangedEventWidget::PostEditImport()
//{
//	Super::PostEditImport();
//	NotifyOnAnyPropertyChanged();
//}

//void UPropertyChangedEventWidget::PostInitProperties()
//{
//	Super::PostInitProperties();
//	NotifyOnAnyPropertyChanged();
//}

#if WITH_EDITOR
void UPropertyChangedEventWidget::PostEditUndo()
{
	Super::PostEditUndo();
	NotifyOnAnyPropertyChanged();
}

void UPropertyChangedEventWidget::PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation)
{
	Super::PostEditUndo(TransactionAnnotation);
	NotifyOnAnyPropertyChanged();
}

void UPropertyChangedEventWidget::PostInterpChange(UProperty* PropertyThatChanged)
{
	Super::PostInterpChange(PropertyThatChanged);
	NotifyOnAnyPropertyChanged();
}

void UPropertyChangedEventWidget::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	NotifyOnAnyPropertyChanged();
}
#endif

void UPropertyChangedEventWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	NotifyOnAnyPropertyChanged();
}

void UPropertyChangedEventWidget::NotifyOnAnyPropertyChanged()
{
	OnAnyPropertyChanged.Broadcast();
	ReceiveOnAnyPropertyChanged();
}
