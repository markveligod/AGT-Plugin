/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Components/SimpleMoveToComponent.h"
#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "GameFramework/Character.h"
#include "Components/SplineComponent.h"

#pragma region ConsoleCheats

#if !UE_BUILD_SHIPPING

static TAutoConsoleVariable<bool> EnableD_SimpleMoveToShowPath(TEXT("SimpleMoveTo.ShowPath"), false, TEXT("SimpleMoveTo.ShowPath [true/false]"), ECVF_Cheat);

#endif

#pragma endregion

#pragma region Default

// Sets default values for this component's properties
USimpleMoveToComponent::USimpleMoveToComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    Spline->ClearSplinePoints();
}

void USimpleMoveToComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (Spline->GetNumberOfSplinePoints() == 0)
    {
        return;
    }

    const FVector ActorLocation = GetOwner()->GetActorLocation();
    DistanceToGoal = (Goal - ActorLocation).Size2D();
    if (DistanceToGoal <= GoalDistanceThreshold)
    {
        RegisterRequestFinished_Event(true);
        return;
    }

    const float DistanceToNextPoint = (NextPoint - ActorLocation).Size2D();
    if (DistanceToNextPoint <= DistanceThreshold)
    {
        CurrentTime += DistanceBetweenPoints;
        NextPoint = Spline->GetLocationAtTime(CurrentTime, ESplineCoordinateSpace::World);
    }

    const FVector Direction = (NextPoint - ActorLocation).GetSafeNormal2D();
    const FVector ActorDirection = GetOwner()->GetActorRotation().Vector().GetSafeNormal2D();
    const float Scale = FMath::Clamp(FVector::DotProduct(ActorDirection, Direction), 0.5f, 1.0f);
    Cast<APawn>(GetOwner())->AddMovementInput(Direction, Scale);

#if !UE_BUILD_SHIPPING

    if (EnableD_SimpleMoveToShowPath.GetValueOnGameThread())
    {
        int32 NumberPoints = Spline->GetNumberOfSplinePoints();
        for (int32 i = 0; i < NumberPoints; ++i)
        {
            FVector WorldPos = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
            DrawDebugSphere(GetWorld(), WorldPos, 32.0f, 12, FColor::Orange, false, 0.0f, 0, 1.0f);
            DrawDebugString(GetWorld(), WorldPos, FString::Printf(TEXT("Number point #%i"), i), nullptr, FColor::Orange, 0.0f, true, 1.0f);

            if (i - 1 >= 0)
            {
                FVector WorldPosPrev = Spline->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::World);
                DrawDebugDirectionalArrow(GetWorld(), WorldPosPrev, WorldPos, 12.0f, FColor::Green, false, 0.0f, 0, 2.0f);
            }
        }
    }

#endif
    
}

#pragma endregion

#pragma region ActionMove

void USimpleMoveToComponent::StartSimpleMove(FVector TargetPos)
{
    if (!Goal.IsNearlyZero()) return;
    APawn* Pawn = GetOwner<APawn>();
    if (!Pawn) return;
    AController* Controller = Pawn->GetController();
    if (!Controller) return;

    Controller->DisableInput(Cast<APlayerController>(Controller));
    Goal = TargetPos;
    TArray<FVector> Points = UAdvanceGameToolLibrary::FindPathToLocationByCharacter(GetWorld(), Goal, Cast<ACharacter>(Pawn));
    if (Points.Num() == 0)
    {
        RegisterRequestFinished_Event(false);
        return;
    }

    for (FVector Point : Points)
    {
        Spline->AddSplineWorldPoint(Point);
        Goal = Point;
    }
    Length = Spline->GetSplineLength();
    Spline->Duration = Length;
    CurrentTime = 0.0f;
    NextPoint = Spline->GetLocationAtTime(CurrentTime, ESplineCoordinateSpace::World);
}

void USimpleMoveToComponent::RegisterRequestFinished_Event(bool bIsSuccess)
{
    if (Goal.IsNearlyZero()) return;

    ResetSimpleMove();
    OnFinishResultSimpleMove.Broadcast(Goal, bIsSuccess);
}

void USimpleMoveToComponent::ResetSimpleMove()
{
    Goal = FVector::ZeroVector;
    DistanceToGoal = 0.0f;
    GoalDistanceThreshold = 100.0f;
    DistanceThreshold = 50.0f;
    DistanceBetweenPoints = 100.0f;
    Spline->ClearSplinePoints();

    const APawn* Pawn = GetOwner<APawn>();
    if (!Pawn) return;
    AController* Controller = Pawn->GetController();
    if (!Controller) return;
    Controller->EnableInput(Cast<APlayerController>(Controller));
}

#pragma endregion
