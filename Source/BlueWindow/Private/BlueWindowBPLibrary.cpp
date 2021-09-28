// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlueWindowBPLibrary.h"

#include "Rendering/DrawElements.h"
#include "Math/TransformCalculus2D.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsWindow.h"
#endif

UBlueWindowBPLibrary::UBlueWindowBPLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UBlueWindowBPLibrary::DrawLinesColored(
    FPaintContext& Context,
    const TArray<FVector2D>& Points,
    const TArray<FLinearColor>& PointColors,
    bool bAntiAlias,
    FLinearColor Tint,
    float Thickness)
{
    Context.MaxLayer++;

    FSlateDrawElement::MakeLines(
        Context.OutDrawElements,
        Context.MaxLayer,
        Context.AllottedGeometry.ToPaintGeometry(),
        Points,
        PointColors,
        ESlateDrawEffect::None,
        Tint,
        bAntiAlias,
        Thickness);
}

void UBlueWindowBPLibrary::DrawBoxBrushed(
    FPaintContext& Context,
    FVector2D Position,
    FVector2D Size,
    FSlateBrush Brush,
    FLinearColor Tint)
{

    Context.MaxLayer++;

    FSlateDrawElement::MakeBox(
        Context.OutDrawElements,
        Context.MaxLayer,
        Context.AllottedGeometry.ToPaintGeometry(Position, Size),
        &Brush,
        ESlateDrawEffect::None,
        Tint);
}

void UBlueWindowBPLibrary::DrawCubicBezierSpline(
    UPARAM(ref) FPaintContext& Context,
    FVector2D P0, FVector2D P1, FVector2D P2, FVector2D P3,
    FLinearColor Tint,
    float Thickness)
{
    Context.MaxLayer++;

    FSlateDrawElement::MakeCubicBezierSpline(
        Context.OutDrawElements,
        Context.MaxLayer,
        Context.AllottedGeometry.ToPaintGeometry(),
        P0, P1, P2, P3,
        Thickness,
        ESlateDrawEffect::None,
        Tint);
}

void UBlueWindowBPLibrary::DrawSpline(
    FPaintContext& Context,
    FVector2D InStart, FVector2D InStartDir,
    FVector2D InEnd, FVector2D InEndDir,
    FLinearColor Tint,
    float Thickness)
{
    Context.MaxLayer++;

    FSlateDrawElement::MakeSpline(
        Context.OutDrawElements,
        Context.MaxLayer,
        Context.AllottedGeometry.ToPaintGeometry(),
        InStart, InStartDir, InEnd, InEndDir,
        Thickness,
        ESlateDrawEffect::None,
        Tint);
}

void UBlueWindowBPLibrary::DrawSplineDrawSpace(
    FPaintContext& Context,
    FVector2D InStart, FVector2D InStartDir,
    FVector2D InEnd, FVector2D InEndDir,
    FLinearColor Tint,
    float Thickness)
{
    Context.MaxLayer++;

    FSlateDrawElement::MakeDrawSpaceSpline(
        Context.OutDrawElements,
        Context.MaxLayer,
        InStart, InStartDir, InEnd, InEndDir,
        Thickness,
        ESlateDrawEffect::None,
        Tint);
}

template <typename TFilterDel>
bool LineTraceFiltered_Internal(UWorld* World, FVector Start, FVector End, TFilterDel Filter, TArray<FHitResult>& OutHits, FHitResult& FirstHit)
{
    OutHits = TArray<FHitResult>();
    TArray<FHitResult> tempHits;

    FCollisionObjectQueryParams QueryParams = FCollisionObjectQueryParams::DefaultObjectQueryParam;
    QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

    FCollisionQueryParams CollisionParams;
    //CollisionParams.AddIgnoredActors(ActorsToIgnore);
    CollisionParams.bReturnFaceIndex = false;
    CollisionParams.bReturnPhysicalMaterial = false;
    CollisionParams.bTraceComplex = true;

    FirstHit = FHitResult();

    float mindist = MAX_flt;

    World->LineTraceMultiByObjectType(
        tempHits, Start, End,
        QueryParams,
        CollisionParams
    );

    for (FHitResult hit : tempHits)
    {
        if (hit.Distance <= 0) continue;
        if (!Filter.Execute(hit)) continue;

        OutHits.Add(hit);
        if (hit.Distance < mindist)
        {
            mindist = hit.Distance;
            FirstHit = hit;
        }
    }
    return OutHits.Num() > 0;
}

bool UBlueWindowBPLibrary::LineTraceFiltered(UWorld* World, FVector Start, FVector End, FTraceResultFilterDelegate Filter, TArray<FHitResult>& OutHits, FHitResult& FirstHit)
{
    return LineTraceFiltered_Internal(World, Start, End, Filter, OutHits, FirstHit);
}

bool UBlueWindowBPLibrary::LineTraceFiltered(UWorld* World, FVector Start, FVector End, FTraceResultFilterStaticDel Filter, TArray<FHitResult>& OutHits, FHitResult& FirstHit)
{
    return LineTraceFiltered_Internal(World, Start, End, Filter, OutHits, FirstHit);
}

FWidgetTransform UBlueWindowBPLibrary::CombineTransform(FWidgetTransform LHS, FWidgetTransform RHS)
{
    return FWidgetTransform(
        LHS.Translation + RHS.Translation,
        LHS.Scale * RHS.Scale,
        FShear2D::FromShearAngles(LHS.Shear + RHS.Shear).GetVector(),
        LHS.Angle + RHS.Angle
    );
}

FWidgetTransform UBlueWindowBPLibrary::InverseTransform(FWidgetTransform Input)
{
    return FWidgetTransform(
        -Input.Translation,
        Input.Scale.Size() == 0.0f ?
            FVector2D(1, 1) :
            FVector2D(1.0f / Input.Scale.X, 1.0f / Input.Scale.Y),
        - Input.Shear,
        - Input.Angle
    );
}

void UBlueWindowBPLibrary::GetAccumulatedWidgetRender(UWidget* Target, FWidgetTransform& Transform, float& Opacity, int MaxDepth)
{
    if(!Target)
    {
        Transform = {};
        Opacity = 1.0;
        return;
    }

    TSharedPtr<SWidget> ParentWidget = Target->GetCachedWidget();
    FSlateRenderTransform CurrTr = {};
    float CurrOp = 1.0f;
    int HierarchyDepth = MaxDepth;

    while (ParentWidget && HierarchyDepth > 0)
    {
        CurrTr = CurrTr.Concatenate(ParentWidget->GetRenderTransform().Get({}));
        CurrOp *= ParentWidget->GetRenderOpacity();

        auto WeakParent = ParentWidget->GetPersistentState().PaintParent;
        if(WeakParent.IsValid())
            ParentWidget = WeakParent.Pin();
        else ParentWidget.Reset();

        HierarchyDepth--;
    }

    Transform = FWidgetTransform(
        CurrTr.GetTranslation(),
        CurrTr.GetMatrix().GetScale().GetVector(),
        {0,0},
        CurrTr.GetMatrix().GetRotationAngle()
    );
    Opacity = CurrOp;
}

void UBlueWindowBPLibrary::SetFocusToGameWindow(bool EnableCapture)
{
#if PLATFORM_WINDOWS
    const Windows::HWND WindowHandle = GetTopWindow(NULL);
    SetFocus(WindowHandle);
    SetCapture(WindowHandle);
#endif
}

Windows::HWND FindTopmostHwnd(Windows::HWND WindowHandle)
{
    Windows::HWND TopMostParent = WindowHandle;
    while (GetParent(TopMostParent) != nullptr)
    {
        TopMostParent = GetParent(TopMostParent);
    }
    return TopMostParent;
}

void UBlueWindowBPLibrary::MoveViewportWindowToZ(UObject* WorldContextObject, EWindowOrder WindowOrder)
{
    if (!WorldContextObject->IsValidLowLevel()) return;
    
#if PLATFORM_WINDOWS
    const Windows::HWND WindowHandle = (HWND)WorldContextObject->GetWorld()->GetGameViewport()->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
    switch (WindowOrder)
    {
    case BOTTOM:
        SetWindowPos(WindowHandle,
            HWND_BOTTOM,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE);
        break;
    case TOP:
        SetWindowPos(WindowHandle,
        HWND_TOP,
        0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetForegroundWindow(WindowHandle);
        SetCapture(WindowHandle);
        break;
    }
#endif
}

void UBlueWindowBPLibrary::DrawLinesThick(
    FPaintContext& Context,
    const TArray<FVector2D>& Points,
    bool bAntiAlias,
    FLinearColor Tint,
    float Thickness)
{
    Context.MaxLayer++;

    FSlateDrawElement::MakeLines(
        Context.OutDrawElements,
        Context.MaxLayer,
        Context.AllottedGeometry.ToPaintGeometry(),
        Points,
        ESlateDrawEffect::None,
        Tint,
        bAntiAlias,
        Thickness);
}
