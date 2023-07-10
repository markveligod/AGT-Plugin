/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#pragma region ActionObject

UObject* UAdvanceGameToolLibrary::SyncAssetLoadObject(TSoftObjectPtr<UObject> AssetObject)
{
    return AssetObject.LoadSynchronous();
}

AActor* UAdvanceGameToolLibrary::SyncSpawnActor(UObject* WorldContextObject, TSoftClassPtr<AActor> AssetClass, FAdvanceActorParameters ActorParameters)
{
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;

    FActorSpawnParameters ActorSpawnParameters;
    ActorSpawnParameters.Name = ActorParameters.Name;
    ActorSpawnParameters.Instigator = ActorParameters.Instigator;
    ActorSpawnParameters.Owner = ActorParameters.Owner;
    ActorSpawnParameters.Template = ActorParameters.Template;
    ActorSpawnParameters.SpawnCollisionHandlingOverride = ActorParameters.SpawnCollisionHandlingOverride;

    return World->SpawnActor(AssetClass.LoadSynchronous(), &ActorParameters.Location, &ActorParameters.Rotation, ActorSpawnParameters);
}

void UAdvanceGameToolLibrary::AsyncAssetLoadObject(TSoftObjectPtr<UObject> AssetObject, const FAsyncAssetLoadSignature& Callback, bool bHighPriority)
{
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    const auto OnLoadedDelegate = FStreamableDelegate::CreateStatic(&UAdvanceGameToolLibrary::OnAssetLoadObjectComplete, AssetObject, Callback);
    Streamable.RequestAsyncLoad(AssetObject.ToSoftObjectPath(), OnLoadedDelegate, bHighPriority ? FStreamableManager::AsyncLoadHighPriority : FStreamableManager::DefaultAsyncLoadPriority);
}

void UAdvanceGameToolLibrary::AsyncSpawnActor(
    UObject* WorldContextObject, TSoftClassPtr<AActor> AssetClass, FAdvanceActorParameters ActorParameters, const FAsyncSpawnActorSignature& Callback, bool bHighPriority)
{
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    const auto OnLoadedDelegate = FStreamableDelegate::CreateStatic(&UAdvanceGameToolLibrary::OnSpawnActorComplete, WorldContextObject, AssetClass, ActorParameters, Callback);
    Streamable.RequestAsyncLoad(AssetClass.ToSoftObjectPath(), OnLoadedDelegate, bHighPriority ? FStreamableManager::AsyncLoadHighPriority : FStreamableManager::DefaultAsyncLoadPriority);
}

void UAdvanceGameToolLibrary::UnloadObject(TSoftObjectPtr<UObject> AssetObject)
{
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    Streamable.Unload(AssetObject.ToSoftObjectPath());
}

void UAdvanceGameToolLibrary::UnloadActor(TSoftClassPtr<AActor> AssetClass)
{
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    Streamable.Unload(AssetClass.ToSoftObjectPath());
}

void UAdvanceGameToolLibrary::DoAsyncTask(FBPAsyncSignature Task, EThreadTaskType ThreadType)
{
    if (!FPlatformProcess::SupportsMultithreading()) return;

    ENamedThreads::Type LThreadType = ENamedThreads::GameThread;

    switch (ThreadType)
    {
        case EThreadTaskType::AnyThread:
        {
            LThreadType = ENamedThreads::AnyThread;
            break;
        }

        case EThreadTaskType::GameThread:
        {
            LThreadType = ENamedThreads::GameThread;
            break;
        }
    }

    AsyncTask(LThreadType, [Task]() { Task.ExecuteIfBound(); });
}

void UAdvanceGameToolLibrary::DoAsyncLongTask(FBPAsyncSignature Task, const FString& ThreadName, EThreadPriorityType ThreadPriority)
{
    if (!FPlatformProcess::SupportsMultithreading()) return;

    TFunction<void()> fp = [Task]() { Task.ExecuteIfBound(); };

    EThreadPriority LThreadPriority = EThreadPriority::TPri_Normal;

    switch (ThreadPriority)
    {
        case EThreadPriorityType::Normal:
        {
            LThreadPriority = EThreadPriority::TPri_Normal;
            break;
        }

        case EThreadPriorityType::AboveNormal:
        {
            LThreadPriority = EThreadPriority::TPri_AboveNormal;
            break;
        }

        case EThreadPriorityType::BelowNormal:
        {
            LThreadPriority = EThreadPriority::TPri_BelowNormal;
            break;
        }

        case EThreadPriorityType::Highest:
        {
            LThreadPriority = EThreadPriority::TPri_Highest;
            break;
        }

        case EThreadPriorityType::Lowest:
        {
            LThreadPriority = EThreadPriority::TPri_Lowest;
            break;
        }
    }

    FLambdaRunnable::RunLambdaOnBackGroundThread(fp, *ThreadName, LThreadPriority);
}

void UAdvanceGameToolLibrary::ParallelForTask(FBPParallelForSignature Task, int32 Num, bool ForceSingleThread, bool PumpRenderingThread)
{
    TFunction<void(int)> fp = [Task](int Val) { Task.ExecuteIfBound(Val); };

    ParallelFor(Num, fp, ForceSingleThread, PumpRenderingThread);
}

void UAdvanceGameToolLibrary::OnAssetLoadObjectComplete(TSoftObjectPtr<UObject> AssetObject, FAsyncAssetLoadSignature Callback)
{
    Callback.ExecuteIfBound(AssetObject.IsValid(), AssetObject.Get());
}

void UAdvanceGameToolLibrary::OnSpawnActorComplete(UObject* WorldContextObject, TSoftClassPtr<AActor> AssetClass, FAdvanceActorParameters ActorParameters, FAsyncSpawnActorSignature Callback)
{
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return;

    FActorSpawnParameters ActorSpawnParameters;
    ActorSpawnParameters.Name = ActorParameters.Name;
    ActorSpawnParameters.Instigator = ActorParameters.Instigator;
    ActorSpawnParameters.Owner = ActorParameters.Owner;
    ActorSpawnParameters.Template = ActorParameters.Template;
    ActorSpawnParameters.SpawnCollisionHandlingOverride = ActorParameters.SpawnCollisionHandlingOverride;

    AActor* Actor = World->SpawnActor(AssetClass.Get(), &ActorParameters.Location, &ActorParameters.Rotation, ActorSpawnParameters);
    Callback.ExecuteIfBound(IsValid(Actor), Actor);
}

#pragma endregion
