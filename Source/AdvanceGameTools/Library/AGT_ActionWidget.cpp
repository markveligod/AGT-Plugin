/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Widget.h"

#pragma region ActionWidget

FVector2D UAdvanceGameToolLibrary::GetMousePositionByWidget(UWidget* Widget)
{
    if (!Widget) return {};
    APlayerController* PC = Widget->GetOwningPlayer();
    if (!PC) return {};

    const FGeometry& Geometry = Widget->GetCachedGeometry();
    FVector2D ZeroPixelPosition;
    FVector2D ZeroViewportPosition;
    USlateBlueprintLibrary::LocalToViewport(PC->GetWorld(), Geometry, FVector2D::ZeroVector, ZeroPixelPosition, ZeroViewportPosition);
    const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(PC->GetWorld());

    return MousePos - ZeroViewportPosition;
}

FVector2D UAdvanceGameToolLibrary::GetMouseUVPosition(UWidget* Widget)
{
    if (!Widget) return {};
    APlayerController* PC = Widget->GetOwningPlayer();
    if (!PC) return {};

    const FGeometry& Geometry = Widget->GetCachedGeometry();

    FVector2D ZeroPixelPosition;
    FVector2D ZeroViewportPosition;
    USlateBlueprintLibrary::LocalToViewport(PC->GetWorld(), Geometry, FVector2D::ZeroVector, ZeroPixelPosition, ZeroViewportPosition);
    FVector2D PixelPosition;
    FVector2D ViewportPosition;
    USlateBlueprintLibrary::LocalToViewport(PC->GetWorld(), Geometry, Geometry.GetLocalSize(), PixelPosition, ViewportPosition);

    const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(PC->GetWorld());
    return (MousePos - ZeroViewportPosition) / (ViewportPosition - ZeroViewportPosition);
}

FVector2D UAdvanceGameToolLibrary::GetMouseUVPositionInvert(UWidget* Widget)
{
    if (!Widget) return {};
    APlayerController* PC = Widget->GetOwningPlayer();
    if (!PC) return {};

    const FGeometry& Geometry = Widget->GetCachedGeometry();

    FVector2D PixelPosition;
    FVector2D ViewportPosition;
    USlateBlueprintLibrary::LocalToViewport(PC->GetWorld(), Geometry, Geometry.GetLocalSize(), PixelPosition, ViewportPosition);

    const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(PC->GetWorld());
    return (ViewportPosition - MousePos) / Geometry.GetLocalSize();
}

#pragma endregion
