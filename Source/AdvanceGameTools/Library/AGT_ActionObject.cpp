/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "LatentActions.h"

struct FLoadAssetPriorityActionBase : FPendingLatentAction
{
    // @TODO: it would be good to have static/global manager?
    FSoftObjectPath SoftObjectPath;
    FStreamableManager StreamableManager;
    TSharedPtr<FStreamableHandle> Handle;
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;

    virtual void OnLoaded() PURE_VIRTUAL(FLoadAssetPriorityActionBase::OnLoaded, );

    FLoadAssetPriorityActionBase(const FSoftObjectPath& InSoftObjectPath, const int32 Priority, const FLatentActionInfo& InLatentInfo)
        : SoftObjectPath(InSoftObjectPath), ExecutionFunction(InLatentInfo.ExecutionFunction), OutputLink(InLatentInfo.Linkage), CallbackTarget(InLatentInfo.CallbackTarget)
    {
        Handle = StreamableManager.RequestAsyncLoad(SoftObjectPath, FStreamableDelegate(), Priority);
    }

    virtual ~FLoadAssetPriorityActionBase() override
    {
        if (Handle.IsValid())
        {
            Handle->ReleaseHandle();
        }
    }

    virtual void UpdateOperation(FLatentResponse& Response) override
    {
        const bool bLoaded = !Handle.IsValid() || Handle->HasLoadCompleted() || Handle->WasCanceled();
        if (bLoaded)
        {
            OnLoaded();
        }
        Response.FinishAndTriggerIf(bLoaded, ExecutionFunction, OutputLink, CallbackTarget);
    }

#if WITH_EDITOR
    virtual FString GetDescription() const override { return FString::Printf(TEXT("Load Asset Priority Action Base: %s"), *SoftObjectPath.ToString()); }
#endif
};

struct FLoadPackagePriorityActionBase : FPendingLatentAction
{
    FString PackagePath;
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;
    EAsyncLoadingResult::Type Result;
    UPackage* LoadedPackage;

    int32 LoadRequest;
    FLoadPackageAsyncDelegate LoadCB;

    virtual void OnLoaded() PURE_VIRTUAL(FLoadPackagePriorityActionBase::OnLoaded, );

    FLoadPackagePriorityActionBase(const FString& packagePath, const int32 priority, const bool blockOnLoad, const FLatentActionInfo& inLatentInfo)
        : PackagePath(packagePath), ExecutionFunction(inLatentInfo.ExecutionFunction), OutputLink(inLatentInfo.Linkage), CallbackTarget(inLatentInfo.CallbackTarget),
          Result(EAsyncLoadingResult::Failed), LoadedPackage(nullptr)
    {
        LoadCB.BindRaw(this, &FLoadPackagePriorityActionBase::OnPackageLoadCompleteCB);
#if ENGINE_MAJOR_VERSION >= 5
        FPackagePath packagePathStruct;
        if (FPackagePath::TryFromPackageName(*PackagePath, packagePathStruct))
        {
            LoadRequest = LoadPackageAsync(packagePathStruct, NAME_None, LoadCB, PKG_None, INDEX_NONE, priority);
        }
        else
        {
            LoadRequest = INDEX_NONE;
        }
#else
        LoadRequest = LoadPackageAsync(PackagePath, nullptr, nullptr, LoadCB, PKG_None, INDEX_NONE, priority);
#endif
        if (LoadRequest != INDEX_NONE)
        {
            if (blockOnLoad)
            {
                FlushAsyncLoading(LoadRequest);
            }
        }
    }

    void OnPackageLoadCompleteCB(const FName& packagePath, UPackage* loadedPackage, EAsyncLoadingResult::Type result)
    {
        Result = result;
        LoadedPackage = loadedPackage;
        LoadRequest = INDEX_NONE;
    }

    virtual void UpdateOperation(FLatentResponse& Response) override
    {
        const bool bLoaded = LoadRequest == INDEX_NONE;
        if (bLoaded)
        {
            OnLoaded();
        }
        Response.FinishAndTriggerIf(bLoaded, ExecutionFunction, OutputLink, CallbackTarget);
    }

#if WITH_EDITOR
    virtual FString GetDescription() const override { return FString::Printf(TEXT("Load Package Priority Action Base: %s"), *PackagePath); }
#endif
};

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

void UAdvanceGameToolLibrary::RemoveLatentActionsForObject(UObject* WorldContextObject, UObject* Object)
{
    if (!GEngine) return;
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        World->GetLatentActionManager().RemoveActionsForObject(Object);
    }
}

void UAdvanceGameToolLibrary::ClearAllTimersForObject(UObject* WorldContextObject, UObject* Object)
{
    if (!GEngine) return;
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        World->GetTimerManager().ClearAllTimersForObject(Object);
    }
}

void UAdvanceGameToolLibrary::CreateObject(TSubclassOf<UObject> objectClass, UObject* outer, UObject*& objectOut)
{
    if (!objectClass)
    {
        objectOut = nullptr;
        return;
    }

    objectOut = NewObject<UObject>(outer ? outer : GetTransientPackage(), objectClass);
}

void UAdvanceGameToolLibrary::DuplicateObject(TSubclassOf<UObject> objectClass, UObject* Object, UObject* outer, UObject*& objectOut)
{
    if (!Object)
    {
        objectOut = nullptr;
        return;
    }
    objectOut = ::DuplicateObject(Object, outer ? outer : Object->GetOuter());
}

bool UAdvanceGameToolLibrary::IsLiveSoftObjectReference(const TSoftObjectPtr<UObject>& SoftObjectReference)
{
    if (SoftObjectReference.IsNull()) return false;

    const TPersistentObjectPtr<FSoftObjectPath> persistObjPtr(SoftObjectReference.ToSoftObjectPath());
    return persistObjPtr.Get(false) != nullptr;
}

FString UAdvanceGameToolLibrary::SoftObjectToString(const TSoftObjectPtr<UObject>& SoftObjectReference)
{
    return SoftObjectReference.ToString();
}

bool UAdvanceGameToolLibrary::IsSoftObjectPending(const TSoftObjectPtr<UObject>& SoftObjectReference)
{
    return SoftObjectReference.IsPending();
}

bool UAdvanceGameToolLibrary::IsSoftObjectNull(const TSoftObjectPtr<UObject>& SoftObjectReference)
{
    return SoftObjectReference.IsNull();
}

bool UAdvanceGameToolLibrary::IsSoftObjectValid(const TSoftObjectPtr<UObject>& SoftObjectReference)
{
    return SoftObjectReference.IsValid();
}

UPackage* UAdvanceGameToolLibrary::FindOrLoadPackage(const FString& PackageName)
{
    UPackage* Pkg = FindPackage(nullptr, *PackageName);
    if (!Pkg)
    {
        Pkg = LoadPackage(nullptr, *PackageName, LOAD_NoWarn | LOAD_Quiet);
    }
    return Pkg;
}

UObject* UAdvanceGameToolLibrary::LoadObjectFromPackage(UPackage* package, const FString& objectName)
{
    if (!package)
    {
        return nullptr;
    }

    return LoadObject<UObject>(package, *objectName);
}

UPackage* UAdvanceGameToolLibrary::GetPackageOfObject(UObject* Object)
{
    if (!Object) return nullptr;

    return Object->GetOutermost();
}

void UAdvanceGameToolLibrary::GetObjectsInPackage(UPackage* package, TArray<UObject*>& ObjectsOut)
{
    if (!package)
    {
        return;
    }

    if (!package->IsFullyLoaded())
    {
        package->FullyLoad();
    }

    for (TObjectIterator<UObject> ObjIt; ObjIt; ++ObjIt)
    {
        UObject* Object = *ObjIt;
        if (Object->IsIn(package))
        {
            ObjectsOut.Add(Object);
        }
    }
}

bool UAdvanceGameToolLibrary::TryConvertFilenameToLongPackageName(const FString& InFilename, FString& OutPackageName, FString& OutFailureReason)
{
    return FPackageName::TryConvertFilenameToLongPackageName(InFilename, OutPackageName, &OutFailureReason);
}

UObject* UAdvanceGameToolLibrary::LoadObjectWithFullPath(const FString& fullObjectPath)
{
    UObject* LoadedObject = StaticLoadObject(UObject::StaticClass(), nullptr, *fullObjectPath, nullptr, LOAD_None, nullptr, true, nullptr);

#if WITH_EDITOR
    // Look at core redirects if we didn't find the Object
    if (!LoadedObject)
    {
        FSoftObjectPath FixupObjectPath = fullObjectPath;
        if (FixupObjectPath.FixupCoreRedirects())
        {
            LoadedObject = LoadObject<UObject>(nullptr, *FixupObjectPath.ToString());
        }
    }
#endif

    while (const UObjectRedirector* Redirector = Cast<UObjectRedirector>(LoadedObject))
    {
        LoadedObject = Redirector->DestinationObject;
    }

    return LoadedObject;
}

void UAdvanceGameToolLibrary::LoadAssetPriority(UObject* WorldContextObject, const TSoftObjectPtr<UObject> Asset, const int32 Priority, FOnAssetLoaded OnLoaded, FLatentActionInfo LatentInfo)
{
    struct FLoadAssetAction : FLoadAssetPriorityActionBase
    {
        FOnAssetLoaded OnLoadedCallback;

        FLoadAssetAction(const FSoftObjectPath& InSoftObjectPath, const int32 Priority, FOnAssetLoaded InOnLoadedCallback, const FLatentActionInfo& InLatentInfo)
            : FLoadAssetPriorityActionBase(InSoftObjectPath, Priority, InLatentInfo), OnLoadedCallback(InOnLoadedCallback)
        {
        }

        virtual void OnLoaded() override
        {
            UObject* LoadedObject = SoftObjectPath.ResolveObject();
            OnLoadedCallback.ExecuteIfBound(LoadedObject);
        }
    };

    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentManager = World->GetLatentActionManager();

        // We always spawn a new load even if this node already queued one, the outside node handles this case
        FLoadAssetAction* NewAction = new FLoadAssetAction(Asset.ToSoftObjectPath(), Priority, OnLoaded, LatentInfo);
        LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
    }
}

void UAdvanceGameToolLibrary::LoadPackagePriority(
    UObject* WorldContextObject, const FString& PackagePath, const int32 Priority, const bool BlockOnLoad, FOnPackageLoaded OnLoaded, FLatentActionInfo LatentInfo)
{
    struct FLoadPackageAction : FLoadPackagePriorityActionBase
    {
        FOnPackageLoaded OnLoadedCallback;

        FLoadPackageAction(const FString& packagePath, const int32 priority, const bool blockOnLoad, FOnPackageLoaded onPackageLoaded, const FLatentActionInfo& inLatentInfo)
            : FLoadPackagePriorityActionBase(packagePath, priority, blockOnLoad, inLatentInfo), OnLoadedCallback(onPackageLoaded)
        {
        }

        virtual void OnLoaded() override { OnLoadedCallback.ExecuteIfBound(LoadedPackage, static_cast<ERyAsyncLoadingResult>(Result)); }
    };

    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        FLatentActionManager& LatentManager = World->GetLatentActionManager();

        // We always spawn a new load even if this node already queued one, the outside node handles this case
        FLoadPackageAction* NewAction = new FLoadPackageAction(PackagePath, Priority, BlockOnLoad, OnLoaded, LatentInfo);
        LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);
    }
}

void UAdvanceGameToolLibrary::RegisterMountPoint(const FString& RootPath, const FString& ContentPath)
{
    FPackageName::RegisterMountPoint(RootPath, ContentPath);
}

void UAdvanceGameToolLibrary::UnRegisterMountPoint(const FString& RootPath, const FString& ContentPath)
{
    FPackageName::UnRegisterMountPoint(RootPath, ContentPath);
}

bool UAdvanceGameToolLibrary::MountPointExists(const FString& RootPath)
{
    return FPackageName::MountPointExists(RootPath);
}

FName UAdvanceGameToolLibrary::GetPackageMountPoint(const FString& InPackagePath, bool InWithoutSlashes)
{
    return FPackageName::GetPackageMountPoint(InPackagePath, InWithoutSlashes);
}

UClass* UAdvanceGameToolLibrary::GetParentClass(UClass* Class)
{
    if (!Class) return nullptr;

    return Class->GetSuperClass();
}

void UAdvanceGameToolLibrary::GetClassHierarchy(UClass* Class, TArray<UClass*>& ClassHierarchy, const bool includeSelf)
{
    UClass* NextClass = Class;
    if (NextClass && includeSelf)
    {
        ClassHierarchy.Add(NextClass);
    }
    while (NextClass && NextClass->GetSuperClass())
    {
        ClassHierarchy.Add(NextClass->GetSuperClass());
        NextClass = NextClass->GetSuperClass();
    }
}

UObject* UAdvanceGameToolLibrary::GetClassDefaultObject(TSubclassOf<UObject> TheClass)
{
    if (!TheClass)
    {
        return nullptr;
    }

    return TheClass->GetDefaultObject();
}

bool UAdvanceGameToolLibrary::SetObjectPropertyValue(UObject* Object, const FName PropertyName, const FString& Value, const bool PrintWarnings)
{
    if (!Object)
    {
        return false;
    }

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 25
    UProperty* FoundProperty = Object->GetClass()->FindPropertyByName(PropertyName);
#else
    FProperty* FoundProperty = Object->GetClass()->FindPropertyByName(PropertyName);
#endif
    if (FoundProperty)
    {
        void* PropertyPtr = FoundProperty->ContainerPtrToValuePtr<void>(Object);
        check(PropertyPtr);
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 25
        if (UNumericProperty* pIntProp = Cast<UNumericProperty>(FoundProperty))
#else
        if (FNumericProperty* pIntProp = CastField<FNumericProperty>(FoundProperty))
#endif
        {
            if (Value.IsNumeric())
            {
                pIntProp->SetNumericPropertyValueFromString(PropertyPtr, *Value);
                return true;
            }
            else
            {
                if (PrintWarnings)
                {
                    WarningLog(FString::Printf(TEXT("SetObjectPropertyValue: Property named '%s' is numeric but the Value string is not"), *PropertyName.ToString()));
                }
                return false;
            }
        }
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 25
        else if (UBoolProperty* pBoolProp = Cast<UBoolProperty>(FoundProperty))
#else
        else if (FBoolProperty* pBoolProp = CastField<FBoolProperty>(FoundProperty))
#endif
        {
            pBoolProp->SetPropertyValue(PropertyPtr, FCString::ToBool(*Value));
            return true;
        }
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 25
        else if (UStructProperty* StructProperty = Cast<UStructProperty>(FoundProperty))
#else
        else if (FStructProperty* StructProperty = CastField<FStructProperty>(FoundProperty))
#endif
        {
            FName StructType = StructProperty->Struct->GetFName();
            if (StructType == NAME_LinearColor)
            {
                FLinearColor* pCol = (FLinearColor*)PropertyPtr;
                return pCol->InitFromString(Value);
            }
            else if (StructType == NAME_Color)
            {
                FColor* pCol = (FColor*)PropertyPtr;
                return pCol->InitFromString(Value);
            }
            else if (StructType == NAME_Vector)
            {
                FVector* pVec = (FVector*)PropertyPtr;
                return pVec->InitFromString(Value);
            }
            else if (StructType == NAME_Rotator)
            {
                FRotator* pRot = (FRotator*)PropertyPtr;
                return pRot->InitFromString(Value);
            }
            else if (StructType == NAME_Transform)
            {
                FTransform* pTrans = (FTransform*)PropertyPtr;
                return pTrans->InitFromString(Value);
            }
        }

        if (PrintWarnings)
        {
            WarningLog(FString::Printf(TEXT("SetObjectPropertyValue: Unsupported property named '%s'"), *PropertyName.ToString()));
        }
    }
    else if (PrintWarnings)
    {
        WarningLog(FString::Printf(TEXT("SetObjectPropertyValue: Unable to find property in Object named '%s'"), *PropertyName.ToString()));
    }

    return false;
}

bool UAdvanceGameToolLibrary::ObjectHasFlag_ArchetypeObject(UObject* Object)
{
    return Object && Object->HasAnyFlags(RF_ArchetypeObject);
}

bool UAdvanceGameToolLibrary::ObjectHasFlag_ClassDefaultObject(UObject* Object)
{
    return Object && Object->HasAnyFlags(RF_ClassDefaultObject);
}

bool UAdvanceGameToolLibrary::ObjectHasFlag_BeginDestroyed(UObject* Object)
{
    return Object && Object->HasAnyFlags(RF_BeginDestroyed);
}

bool UAdvanceGameToolLibrary::ObjectHasFlag_FinishDestroyed(UObject* Object)
{
    return Object && Object->HasAnyFlags(RF_FinishDestroyed);
}

bool UAdvanceGameToolLibrary::ObjectHasFlag_WasLoaded(UObject* Object)
{
    return Object && Object->HasAnyFlags(RF_WasLoaded);
}

#pragma endregion
