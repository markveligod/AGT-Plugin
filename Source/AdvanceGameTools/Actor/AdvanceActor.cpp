/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Actor/AdvanceActor.h"
#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "Components/BillboardComponent.h"

#pragma region DefaultInterface

// Sets default values
AAdvanceActor::AAdvanceActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(FName("Root scene component"));
    SetRootComponent(RootScene.Get());

    BillboardScene = CreateDefaultSubobject<UBillboardComponent>(FName("Billboard component"));
    BillboardScene->SetupAttachment(RootScene.Get());
}

// Called every frame
void AAdvanceActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bEnableDrawArrowActor)
    {
        DrawArrowDirectionActor(0.0f);
    }

    if (bEnableDrawBoxActor)
    {
        DrawBoxActor(0.0f);
    }
}

// Called when the game starts or when spawned
void AAdvanceActor::BeginPlay()
{
    Super::BeginPlay();
}

void AAdvanceActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (bEnableEditorTick)
    {
        if (!CHECK_VALID_TIMER(GetWorld(), EditorTickTimerHandle))
        {
            const float TimerSecond = 1.0f / TickRatePerSecond;
            const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::PreEditorTick, TimerSecond);
            EditorTickTimerHandle = QUICK_TIMER_WITH_DELEGATE(GetWorld(), TimerDelegate, TimerSecond, true);
        }
    }
    else
    {
        if (CHECK_VALID_TIMER(GetWorld(), EditorTickTimerHandle))
        {
            CLEAR_TIMER(GetWorld(), EditorTickTimerHandle);
        }
    }
}

void AAdvanceActor::PreEditorTick(float DeltaTime)
{
    EditorTick(DeltaTime);
    EditorTick_Event(DeltaTime);
}

void AAdvanceActor::EditorTick(float DeltaTime)
{
    if (bEnableDrawArrowActor)
    {
        DrawArrowDirectionActor(DeltaTime);
    }

    ReplaceBillboardPosition();

    if (bEnableDrawBoxActor)
    {
        DrawBoxActor(DeltaTime);
    }
}

#pragma endregion

#pragma region ActionDrawDebug

void AAdvanceActor::DrawArrowDirectionActor(float DeltaTime) const
{
    const FVector EndForwardPoint = GetActorLocation() + GetActorForwardVector() * DirectionArrowDebugData.ArrowDistance;
    DrawDebugDirectionalArrow(
        GetWorld(), GetActorLocation(), EndForwardPoint, DirectionArrowDebugData.ArrowSize, DirectionArrowDebugData.ForwardArrowColor, false, DeltaTime, 0, DirectionArrowDebugData.Thickness);
    DrawDebugString(GetWorld(), EndForwardPoint, TEXT("Forward Arrow"), nullptr, DirectionArrowDebugData.ForwardArrowColor, DeltaTime, true);

    const FVector EndInvertForwardPoint = GetActorLocation() + GetActorForwardVector() * -1.0f * DirectionArrowDebugData.ArrowDistance;
    DrawDebugDirectionalArrow(
        GetWorld(), GetActorLocation(), EndInvertForwardPoint, DirectionArrowDebugData.ArrowSize, DirectionArrowDebugData.InvForwardArrowColor, false, DeltaTime, 0, DirectionArrowDebugData.Thickness);
    DrawDebugString(GetWorld(), EndInvertForwardPoint, TEXT("Invert Forward Arrow"), nullptr, DirectionArrowDebugData.InvForwardArrowColor, DeltaTime, true);

    const FVector EndRightPoint = GetActorLocation() + GetActorRightVector() * DirectionArrowDebugData.ArrowDistance;
    DrawDebugDirectionalArrow(
        GetWorld(), GetActorLocation(), EndRightPoint, DirectionArrowDebugData.ArrowSize, DirectionArrowDebugData.RightArrowColor, false, DeltaTime, 0, DirectionArrowDebugData.Thickness);
    DrawDebugString(GetWorld(), EndRightPoint, TEXT("Right Arrow"), nullptr, DirectionArrowDebugData.RightArrowColor, DeltaTime, true);

    const FVector EndInvertRightPoint = GetActorLocation() + GetActorRightVector() * -1.0f * DirectionArrowDebugData.ArrowDistance;
    DrawDebugDirectionalArrow(
        GetWorld(), GetActorLocation(), EndInvertRightPoint, DirectionArrowDebugData.ArrowSize, DirectionArrowDebugData.InvRightArrowColor, false, DeltaTime, 0, DirectionArrowDebugData.Thickness);
    DrawDebugString(GetWorld(), EndInvertRightPoint, TEXT("Invert Right Arrow"), nullptr, DirectionArrowDebugData.InvRightArrowColor, DeltaTime, true);
}

void AAdvanceActor::ReplaceBillboardPosition() const
{
    FVector Origin;
    FVector BoxExtent;
    GetActorBounds(true, Origin, BoxExtent, true);
    const float MaxVector = BoxExtent.GetMax();

    if (BillboardScene)
    {
        FVector RelPos = BillboardScene->GetRelativeLocation();
        RelPos.Z = MaxVector;
        BillboardScene->SetRelativeLocation(RelPos);
    }
}

void AAdvanceActor::DrawBoxActor(float DeltaTime) const
{
    FVector Origin;
    FVector BoxExtent;
    GetActorBounds(true, Origin, BoxExtent, true);
    const FVector ExBox = BoxActorDebugData.DefaultSizeBox.GetMax() > BoxExtent.GetMax() ? BoxActorDebugData.DefaultSizeBox : BoxExtent;

    DrawDebugBox(GetWorld(), Origin, ExBox, BoxActorDebugData.BoxColor, false, DeltaTime, 0, BoxActorDebugData.Thickness);
}

#pragma endregion
