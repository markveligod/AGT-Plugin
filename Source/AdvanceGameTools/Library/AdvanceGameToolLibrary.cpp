/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "Misc/OutputDeviceHelper.h"
#include "GameFramework/Character.h"

#if !UE_BUILD_SHIPPING

#include "Logging/LogVerbosity.h"

static TAutoConsoleVariable<bool> EnableD_ShowLogging(TEXT("ShowLogging"), false, TEXT("Shows logging on the current hood of the player"), ECVF_Cheat);

class OwnerOutputDevice : public FOutputDevice
{
public:
    OwnerOutputDevice()
    {
        check(GLog);
        GLog->AddOutputDevice(this);
    }

    ~OwnerOutputDevice()
    {
        if (GLog != nullptr)
        {
            GLog->RemoveOutputDevice(this);
        }
    }

protected:
    virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const class FName& Category) override
    {
        if (!GEngine) return;
        if (!GWorld) return;
        if (!GWorld->IsGameWorld()) return;
        if (!GWorld->IsInitialized()) return;
        if (!EnableD_ShowLogging.GetValueOnAnyThread()) return;

        const FString Result = FOutputDeviceHelper::FormatLogLine(Verbosity, Category, Message);
        FColor Color;
        if (Verbosity == ELogVerbosity::Display)
        {
            Color = FColor::White;
        }
        if (Verbosity == ELogVerbosity::Warning)
        {
            Color = FColor::Yellow;
        }
        if (Verbosity == ELogVerbosity::Error)
        {
            Color = FColor::Red;
        }
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, Color, Result);
    }
};
static TSharedPtr<OwnerOutputDevice> OwnerOutput = MakeShareable(new OwnerOutputDevice);

static FAutoConsoleCommandWithWorldAndArgs EnableD_PlatformShowStat(TEXT("Platform.ShowStat"), TEXT("Platform.ShowStat [Delay=15]"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](const TArray<FString>& Args, UWorld* World)
        {
            if (!GEngine) return;
            const float Delay = Args.IsValidIndex(0) ? FCString::Atof(*Args[0]) : 15.0f;
            FString Result;
            Result.Append(FString::Printf(TEXT("Project Name: [%s]\n"), *UAdvanceGameToolLibrary::GetProjectName()));
            Result.Append(FString::Printf(TEXT("Project Version: [%s]\n"), *UAdvanceGameToolLibrary::GetProjectVersion()));
            Result.Append(FString::Printf(TEXT("Company Name: [%s]\n"), *UAdvanceGameToolLibrary::GetCompanyName()));
            Result.Append(FString::Printf(TEXT("Copyright Notice: [%s]\n"), *UAdvanceGameToolLibrary::GetCopyrightNotice()));
            Result.Append(FString::Printf(TEXT("--- | --- | ---\n")));
            Result.Append(FString::Printf(TEXT("OS version: [%s]\n"), *UAdvanceGameToolLibrary::GetOSVersion()));
            Result.Append(FString::Printf(TEXT("Audio brand name: [%s]\n"), *UAdvanceGameToolLibrary::GetAudioDeviceName()));
            Result.Append(FString::Printf(TEXT("CPU brand name: [%s]\n"), *UAdvanceGameToolLibrary::GetCPUBrandName()));
            Result.Append(FString::Printf(TEXT("CPU Vendor name: [%s]\n"), *UAdvanceGameToolLibrary::GetCPUVendorName()));
            Result.Append(FString::Printf(TEXT("CPU cores: [%i]\n"), UAdvanceGameToolLibrary::GetCPUCores()));
            Result.Append(FString::Printf(TEXT("Motherboard name: [%s]\n"), *UAdvanceGameToolLibrary::GetMotherboardName()));
            Result.Append(FString::Printf(TEXT("GPU brand name: [%s]\n"), *UAdvanceGameToolLibrary::GetGPUBrandName()));
            Result.Append(FString::Printf(TEXT("Total memory: %s\n"), *UAdvanceGameToolLibrary::GetTotalPhysicalGB()));
            Result.Append(FString::Printf(TEXT("Monitors count: [%i]\n"), UAdvanceGameToolLibrary::GetMonitorsCount()));
            Result.Append(FString::Printf(TEXT("Primary monitor: [%i]\n"), UAdvanceGameToolLibrary::GetPrimaryMonitor()));

            GEngine->AddOnScreenDebugMessage(-1, Delay, FColor::Orange, Result);
        }));

#endif
