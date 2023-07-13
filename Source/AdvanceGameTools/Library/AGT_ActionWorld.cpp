/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "Kismet/GameplayStatics.h"

#pragma region ActionWorld

FTimerHandle UAdvanceGameToolLibrary::SetTimerByEventForWorld(
    UObject* WorldContextObject, FTimerDynamicDelegate Delegate, float Time, bool bLooping, float InitialStartDelay, float InitialStartDelayVariance)
{
    FTimerHandle Handle;
    if (Delegate.IsBound())
    {
        const UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        if (World)
        {
            InitialStartDelay += FMath::RandRange(-InitialStartDelayVariance, InitialStartDelayVariance);
            if (Time <= 0.f || (Time + InitialStartDelay) < 0.f)
            {
                FString ObjectName = GetNameSafe(Delegate.GetUObject());
                FString FunctionName = Delegate.GetFunctionName().ToString();
                FFrame::KismetExecutionMessage(*FString::Printf(TEXT("%s %s SetTimer passed a negative or zero time. The associated timer may fail to be created/fire! If using "
                                                                     "InitialStartDelayVariance, be sure it is smaller than (Time + InitialStartDelay)."),
                                                   *ObjectName, *FunctionName),
                    ELogVerbosity::Warning);
            }

            FTimerManager& TimerManager = World->GetTimerManager();
            Handle = TimerManager.K2_FindDynamicTimerHandle(Delegate);
            TimerManager.SetTimer(Handle, Delegate, Time, bLooping, (Time + InitialStartDelay));
        }
    }
    else
    {
        UE_LOG(LogBlueprintUserMessages, Warning, TEXT("SetTimer passed a bad function (%s) or object (%s)"), *Delegate.GetFunctionName().ToString(), *GetNameSafe(Delegate.GetUObject()));
    }

    return Handle;
}

FTimerHandle UAdvanceGameToolLibrary::SetTimerByFunctionForWorld(
    UObject* WorldContextObject, UBlueprint* Object, FString FunctionName, float Time, bool bLooping, float InitialStartDelay, float InitialStartDelayVariance)
{
    FName const FunctionFName(*FunctionName);

    if (Object)
    {
        UFunction* const Func = Object->FindFunction(FunctionFName);
        if (Func && (Func->ParmsSize > 0))
        {
            // User passed in a valid function, but one that takes parameters
            // FTimerDynamicDelegate expects zero parameters and will choke on execution if it tries
            // to execute a mismatched function
            UE_LOG(LogBlueprintUserMessages, Warning, TEXT("SetTimer passed a function (%s) that expects parameters."), *FunctionName);
            return FTimerHandle();
        }
    }

    FTimerDynamicDelegate Delegate;
    Delegate.BindUFunction(Object, FunctionFName);
    return SetTimerByEventForWorld(WorldContextObject, Delegate, Time, bLooping, InitialStartDelay, InitialStartDelayVariance);
}

FTimerHandle UAdvanceGameToolLibrary::QuickTimerByTimerDelegate(const UWorld* World, const FTimerDelegate& TimerDelegate, float Timer, bool bLooping)
{
    if (World)
    {
        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, Timer, bLooping);
        return TimerHandle;
    }
    return {};
}

void UAdvanceGameToolLibrary::QuickTimerNextTickByTimerDelegate(const UWorld* World, const FTimerDelegate& TimerDelegate)
{
    if (World)
    {
        World->GetTimerManager().SetTimerForNextTick(TimerDelegate);
    }
}

void UAdvanceGameToolLibrary::ClearTimerHandle(const UWorld* World, FTimerHandle& TimerHandle)
{
    if (World)
    {
        World->GetTimerManager().ClearTimer(TimerHandle);
    }
}

bool UAdvanceGameToolLibrary::CheckValidTimerHandle(const UWorld* World, const FTimerHandle& TimerHandle)
{
    if (World)
    {
        return World->GetTimerManager().TimerExists(TimerHandle);
    }
    return false;
}

void UAdvanceGameToolLibrary::AsyncLevelLoad(UObject* WorldContextObject, const FString& LevelDir, const FString& LevelName, const FString& Options)
{
    LoadPackageAsync(LevelDir + LevelName, FLoadPackageAsyncDelegate::CreateStatic(&AsyncLevelLoadFinished, WorldContextObject, Options), 0, PKG_ContainsMap);
}

void UAdvanceGameToolLibrary::AsyncLevelLoadByObjectReference(UObject* WorldContextObject, TSoftObjectPtr<UWorld> WorldSoftObject, const FString& Options)
{
    const FString LevelPath = WorldSoftObject.ToString();
    LoadPackageAsync(LevelPath, FLoadPackageAsyncDelegate::CreateStatic(&AsyncLevelLoadFinished, WorldContextObject, Options), 0, PKG_ContainsMap);
}

void UAdvanceGameToolLibrary::AsyncLevelLoadFinished(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result, UObject* WorldContextObject, FString Options)
{
    if (Result == EAsyncLoadingResult::Succeeded && LoadedPackage)
    {
        UGameplayStatics::OpenLevel(WorldContextObject, PackageName, true, Options);
    }
}

#pragma endregion
