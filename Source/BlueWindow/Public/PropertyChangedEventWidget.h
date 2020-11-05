// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

#include "PropertyChangedEventWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnyPropertyChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEditorTickDelegate, float, DeltaTime);

class UWidgetAnimation;

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
	//virtual void PostEditImport() override;
	//virtual void PostInitProperties() override;

#if WITH_EDITOR
	static FTimerDelegate OnTickDel;

	virtual void PostEditUndo() override;
	virtual void PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation) override;
	// virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostInterpChange(FProperty* PropertyThatChanged) override;

	virtual void OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs) override;
	static void OnEditorTickTrigger(float DeltaTime);
	static TSet<TSoftObjectPtr<UPropertyChangedEventWidget>> AllPropChangedWidgets;
	static TSet<TSoftObjectPtr<UPropertyChangedEventWidget>> RemovablePropChangedWidgets;
	bool Initialize() override;

	void BeginDestroy() override;
#endif

	virtual void SynchronizeProperties() override;

	/**
	 * Seek animation and evaluate it at specified time
	 *
	 * @param Animation			Input animation
	 * @param Time				Target time in seconds
	 * @param bPrimeSession		Make it true while animating, false when not anymore
	 * @param bRestoreState		Restore animated parameters after session has ended.
	 */
	UFUNCTION(BlueprintCallable, Category="BlueWindow")
	void JumpToTime(UWidgetAnimation* Animation, float Time, bool bPrimeSession, bool bRestoreState);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
	FAnyPropertyChangedDelegate OnAnyPropertyChanged;

	void NotifyOnAnyPropertyChanged();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AnyPropertyChanged"), Category = "BlueWindow")
	void ReceiveOnAnyPropertyChanged();


	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
	FEditorTickDelegate OnEditorTick;

	void NotifyOnEditorTick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EditorTick"), Category = "BlueWindow")
	void ReceiveOnEditorTick(float DeltaTime);

};
