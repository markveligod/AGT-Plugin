/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "NavigationSystem.h"
#include "Algo/ForEach.h"
#include "GameFramework/Character.h"

#pragma region NavSystem

void UAdvanceGameToolLibrary::ReloadNavMeshByWorld(UObject* WorldContextObject)
{
    if (!WorldContextObject) return;
    if (UWorld* World = WorldContextObject->GetWorld())
    {
        UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
        UAdvanceGameToolLibrary::ReloadNavMesh(NavSys);
    }
}

void UAdvanceGameToolLibrary::ReloadNavMesh(UNavigationSystemV1* NavSystem)
{
    if (NavSystem)
    {
        NavSystem->Build();
    }
}

void UAdvanceGameToolLibrary::ReloadNavMeshes(TArray<UNavigationSystemV1*> NavSystemArray)
{
    Algo::ForEach(NavSystemArray, &UAdvanceGameToolLibrary::ReloadNavMesh);
}

TArray<FVector> UAdvanceGameToolLibrary::FindPathToLocationByCharacter(UObject* WorldContextObject, FVector Location, ACharacter* Character)
{
    TArray<FVector> Result;

    if (!WorldContextObject) return Result;
    if (!Character) return Result;
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return Result;
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!NavSys) return Result;

    FNavPathSharedPtr Path;
    const ANavigationData* NavData = NavSys->GetNavDataForProps(Character->GetNavAgentPropertiesRef(), Character->GetNavAgentLocation());
    if (NavData)
    {
        const FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, Character, nullptr);
        FPathFindingQuery Query = FPathFindingQuery(*Character, *NavData, Character->GetNavAgentLocation(), Location, NavFilter);
        Query.SetAllowPartialPaths(true);

        FPathFindingResult PathResult = NavSys->FindPathSync(Query);
        if (PathResult.Result != ENavigationQueryResult::Error)
        {
            if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
            {
                PathResult.Path->EnableRecalculationOnInvalidation(true);
                Path = PathResult.Path;
            }
        }
    }

    if (Path)
    {
        const TArray<FNavPathPoint>& Points = Path->GetPathPoints();
        for (int PointIndex = 0; PointIndex < Points.Num(); ++PointIndex)
        {
            Result.Add(Points[PointIndex].Location);
        }
    }
    return Result;
}

#pragma endregion
