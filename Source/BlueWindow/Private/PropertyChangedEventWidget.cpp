// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "PropertyChangedEventWidget.h"

#include "PropertyBinding.h"

#include "UObject/UnrealType.h"

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

#include "Editor.h"
#include "Misc/App.h"

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

void UPropertyChangedEventWidget::PostInterpChange(FProperty* PropertyThatChanged)
{
	Super::PostInterpChange(PropertyThatChanged);
	NotifyOnAnyPropertyChanged();
}

void UPropertyChangedEventWidget::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	NotifyOnAnyPropertyChanged();
}

FTimerDelegate UPropertyChangedEventWidget::OnTickDel;
TSet<TSoftObjectPtr<UPropertyChangedEventWidget>> UPropertyChangedEventWidget::AllPropChangedWidgets;
TSet<TSoftObjectPtr<UPropertyChangedEventWidget>> UPropertyChangedEventWidget::RemovablePropChangedWidgets;

bool UPropertyChangedEventWidget::Initialize()
{
	Super::Initialize();
	if (!GEditor) return true;

	if (!OnTickDel.IsBound())
	{
		OnTickDel = OnTickDel.CreateLambda([this]()
		{
			OnEditorTickTrigger(FApp::GetDeltaTime());
		});

		GEditor->GetTimerManager().Get().SetTimerForNextTick(OnTickDel);
	}

	AllPropChangedWidgets.Add(TSoftObjectPtr<UPropertyChangedEventWidget>(this));

	return true;
}

void UPropertyChangedEventWidget::OnEditorTickTrigger(float DeltaTime)
{
	RemovablePropChangedWidgets.Empty();

	for(auto widget : AllPropChangedWidgets)
	{
		if (widget.IsValid())
		{
			UWorld* const World = GEngine->GetWorldFromContextObject(widget.Get(), EGetWorldErrorMode::ReturnNull);
			if(World)
			{
				if (World->WorldType != EWorldType::PIE)
					widget->NotifyOnEditorTick(DeltaTime);
			}
			else widget->NotifyOnEditorTick(DeltaTime);
		}
		else
		{
			RemovablePropChangedWidgets.Add(widget);
			UE_LOG(LogTemp, Display, TEXT("a PropertyChangedEvent widget was Removed"));
		}
	}

	for(auto widget : RemovablePropChangedWidgets)
	{
		AllPropChangedWidgets.Remove(widget);
	}

	GEditor->GetTimerManager().Get().SetTimerForNextTick(OnTickDel);
}

void UPropertyChangedEventWidget::BeginDestroy()
{
	Super::BeginDestroy();
	if (this->GetPathName().Contains(TEXT("None.None"))) return;
	if (this->GetName() == TEXT("None")) return;

	AllPropChangedWidgets.Remove(TSoftObjectPtr<UPropertyChangedEventWidget>(this));
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

void UPropertyChangedEventWidget::NotifyOnEditorTick(float DeltaTime)
{
	OnEditorTick.Broadcast(DeltaTime);
	ReceiveOnEditorTick(DeltaTime);
}
