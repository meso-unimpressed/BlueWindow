// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "PropertyChangedEventWidget.h"

#include "PropertyBinding.h"
#include "UMGSequencePlayer.h"


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

#include "Misc/App.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"

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

bool UPropertyChangedEventWidget::Initialize()
{
	Super::Initialize();
	if (!GUnrealEd->OnPostEditorTick().IsBoundToObject(this))
		EditorTickHandle = GUnrealEd->OnPostEditorTick().AddUObject(this, &UPropertyChangedEventWidget::NotifyOnEditorTick);
	return true;
}

void UPropertyChangedEventWidget::BeginDestroy()
{
	Super::BeginDestroy();

	if (GUnrealEd->OnPostEditorTick().IsBoundToObject(this))
	{
		GUnrealEd->OnPostEditorTick().Remove(EditorTickHandle);
	}
	EditorTickHandle.Reset();

	//if (this->GetPathName().Contains(TEXT("None.None"))) return;
	//if (this->GetName() == TEXT("None")) return;
}

#endif

void UPropertyChangedEventWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	NotifyOnAnyPropertyChanged();
}

void UPropertyChangedEventWidget::JumpToTime(UWidgetAnimation* Animation, float Time, bool bPrimeSession, bool bRestoreState)
{
    UUMGSequencePlayer* Player = GetOrAddSequencePlayer(Animation);
	if(!Player) return;

	if (!Player->GetEvaluationTemplate().IsValid() && bPrimeSession)
	{
		Player->GetEvaluationTemplate().Initialize(*Animation, *Player);
	}
	if(Player->GetEvaluationTemplate().IsValid())
	{
	    auto AnimRes = Animation->GetMovieScene()->GetTickResolution();
	    FFrameTime FrameTime = Time * AnimRes;

	    FMovieSceneContext Context(
		    FMovieSceneEvaluationRange(FrameTime, AnimRes),
		    EMovieScenePlayerStatus::Jumping
	    );
	    Context.SetHasJumped(true);
		Player->GetEvaluationTemplate().Evaluate(Context, *Player);

		if(!bPrimeSession)
		{
			Player->GetEvaluationTemplate().Finish(*Player);

			if(bRestoreState)
			    Player->RestorePreAnimatedState();
		}
	}
}

void UPropertyChangedEventWidget::NotifyOnAnyPropertyChanged()
{
	OnAnyPropertyChanged.Broadcast();
	ReceiveOnAnyPropertyChanged();
}

void UPropertyChangedEventWidget::NotifyOnEditorTick(float DeltaTime)
{
    if(!this->IsValidLowLevelFast()) return;
	OnEditorTick.Broadcast(DeltaTime);
	ReceiveOnEditorTick(DeltaTime);
}
