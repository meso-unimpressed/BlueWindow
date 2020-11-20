

#include "InteractiveRetainerBox.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#include "Slate/SRetainerWidget.h"

#define LOCTEXT_NAMESPACE "BlueWindow"

static FName DefaultTextureParameterName("Texture");

/////////////////////////////////////////////////////
// UInteractiveRetainerBox

UInteractiveRetainerBox::UInteractiveRetainerBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Visibility = ESlateVisibility::Visible;
	Phase = 0;
	PhaseCount = 1;
	RenderOnPhase = true;
	RenderOnInvalidation = false;
	TextureParameter = DefaultTextureParameterName;
}

void UInteractiveRetainerBox::SetRenderingPhase(int PhaseToRenderOn, int32 TotalRenderingPhases)
{
	Phase = PhaseToRenderOn;
	PhaseCount = TotalRenderingPhases;
	
	if ( PhaseCount < 1 )
	{
		PhaseCount = 1;
	}

	if (MyRetainerWidget.IsValid())
	{
		MyRetainerWidget->SetRenderingPhase(Phase, PhaseCount);
	}
}

void UInteractiveRetainerBox::RequestRender()
{
	if ( MyRetainerWidget.IsValid() )
	{
		MyRetainerWidget->RequestRender();
	}
}

UMaterialInstanceDynamic* UInteractiveRetainerBox::GetEffectMaterial() const
{
	if ( MyRetainerWidget.IsValid() )
	{
		return MyRetainerWidget->GetEffectMaterial();
	}

	return nullptr;
}

void UInteractiveRetainerBox::SetEffectMaterial(UMaterialInterface* InEffectMaterial)
{
	EffectMaterial = InEffectMaterial;
	if ( MyRetainerWidget.IsValid() )
	{
		MyRetainerWidget->SetEffectMaterial(EffectMaterial);
	}
}

void UInteractiveRetainerBox::SetTextureParameter(FName InTextureParameter)
{
	TextureParameter = InTextureParameter;
	if ( MyRetainerWidget.IsValid() )
	{
		MyRetainerWidget->SetTextureParameter(TextureParameter);
	}
}

UTexture* UInteractiveRetainerBox::GetTexture()
{
    auto effectMat = GetEffectMaterial();
    if(!effectMat) return nullptr;
	UTexture* res;
	effectMat->GetTextureParameterValue({TextureParameter}, res);
	return res;
}

void UInteractiveRetainerBox::ResetContentTransform()
{
	if(!GetContentSlot()) return;

	TWeakPtr<SWidget> ParentWidget = MyRetainerWidget;
	FSlateRenderTransform CurrTr = {};
	float CurrOp = 1.0f;
	int HierarchyDepth = 100;

    while (ParentWidget.IsValid() && HierarchyDepth > 0)
    {
        CurrTr = CurrTr.Concatenate(ParentWidget.Pin()->GetRenderTransform().Get({}));
		CurrOp *= ParentWidget.Pin()->GetRenderOpacity();
		ParentWidget = ParentWidget.Pin()->GetPersistentState().PaintParent;
		HierarchyDepth--;
    }

	auto content = GetContentSlot()->Content->TakeWidget();
	content->SetRenderTransform(CurrTr.Inverse());
	content->SetRenderOpacity(1/FMath::Max(CurrOp, 0.01f));
}

void UInteractiveRetainerBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyRetainerWidget.Reset();
}

TSharedRef<SWidget> UInteractiveRetainerBox::RebuildWidget()
{
	MyRetainerWidget =
		SNew(SRetainerWidget)
		.RenderOnInvalidation(RenderOnInvalidation)
		.RenderOnPhase(RenderOnPhase)
		.Phase(Phase)
		.PhaseCount(PhaseCount)
#if STATS
		.StatId( FName( *FString::Printf(TEXT("%s [%s]"), *GetFName().ToString(), *GetClass()->GetName() ) ) )
#endif//STATS
		;
	MyRetainerWidget->SetRetainedRendering(PreviewInDesignTime);
	
	if ( GetChildrenCount() > 0 )
	{
		MyRetainerWidget->SetContent(GetContentSlot()->Content ? GetContentSlot()->Content->TakeWidget() : SNullWidget::NullWidget);
		ResetContentTransform();
	}
	
	return MyRetainerWidget.ToSharedRef();
}

void UInteractiveRetainerBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyRetainerWidget->SetEffectMaterial(EffectMaterial);
	MyRetainerWidget->SetTextureParameter(TextureParameter);
	MyRetainerWidget->SetWorld(GetWorld());
	MyRetainerWidget->SetRetainedRendering(PreviewInDesignTime);
	ResetContentTransform();
}

void UInteractiveRetainerBox::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live slot if it already exists
	if ( MyRetainerWidget.IsValid() )
	{
		MyRetainerWidget->SetContent(InSlot->Content ? InSlot->Content->TakeWidget() : SNullWidget::NullWidget);
		ResetContentTransform();
	}
}

void UInteractiveRetainerBox::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if ( MyRetainerWidget.IsValid() )
	{
		MyRetainerWidget->SetContent(SNullWidget::NullWidget);
	}
}

#if WITH_EDITOR

const FText UInteractiveRetainerBox::GetPaletteCategory()
{
	return LOCTEXT("Optimization", "Optimization");
}

void UInteractiveRetainerBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	RebuildWidget();
    InvalidateLayoutAndVolatility();
	ResetContentTransform();
}

#endif

FGeometry UInteractiveRetainerBox::GetCachedAllottedGeometry() const
{
	if (MyRetainerWidget.IsValid())
	{
		return MyRetainerWidget->GetTickSpaceGeometry();
	}

	static const FGeometry TempGeo;
	return TempGeo;
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
