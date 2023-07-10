/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "AudioDevice.h"
#include "GeneralProjectSettings.h"

#if PLATFORM_WINDOWS
#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#endif

#pragma region ActionPlatform

FString UAdvanceGameToolLibrary::GetCPUBrandName()
{
    return FPlatformMisc::GetCPUBrand();
}

FString UAdvanceGameToolLibrary::GetCPUVendorName()
{
    return FPlatformMisc::GetCPUVendor();
}

FString UAdvanceGameToolLibrary::GetGPUBrandName()
{
    return FPlatformMisc::GetPrimaryGPUBrand();
}

int32 UAdvanceGameToolLibrary::GetCPUCores()
{
    return FPlatformMisc::NumberOfCores();
}

FString UAdvanceGameToolLibrary::GetAudioDeviceName()
{
    FString DeviceName = "";
    TArray<FString> DeviceNames;

    if (GEngine)
    {
        TArray<FAudioDevice*> AudioDevices = GEngine->GetAudioDeviceManager()->GetAudioDevices();

        if (AudioDevices.Num() > 0)
        {
            AudioDevices[0]->GetAudioDeviceList(DeviceNames);

            if (DeviceNames.Num() > 0)
            {
                DeviceName = DeviceNames[0];
            }
        }
    }

    return DeviceName;
}

int32 UAdvanceGameToolLibrary::GetMonitorsCount()
{
    FDisplayMetrics Display;
    FDisplayMetrics::RebuildDisplayMetrics(Display);

    return Display.MonitorInfo.Num();
}

int32 UAdvanceGameToolLibrary::GetPrimaryMonitor()
{
    FDisplayMetrics Display;
    FDisplayMetrics::RebuildDisplayMetrics(Display);

    int32 MonitorNumber = 0;
    FParse::Value(FCommandLine::Get(), TEXT("monitor="), MonitorNumber);

    int32 MonitorIndex = INDEX_NONE;
    if (MonitorNumber == 0)
    {
        for (int32 Index = 0; Index < Display.MonitorInfo.Num(); Index++)
        {
            if (Display.MonitorInfo[Index].bIsPrimary)
            {
                MonitorIndex = Index;
                break;
            }
        }
    }
    else
    {
        MonitorIndex = MonitorNumber - 1;
    }

    return MonitorIndex;
}

FString UAdvanceGameToolLibrary::GetOSVersion()
{
    return FPlatformMisc::GetOSVersion();
}

FString UAdvanceGameToolLibrary::GetTotalPhysicalGB()
{
    return FString::Printf(TEXT("[%i] GB"), FPlatformMemory::GetConstants().TotalPhysicalGB);
}

FString UAdvanceGameToolLibrary::GetMotherboardName()
{
    FString Result;

#if PLATFORM_WINDOWS
    FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System\\BIOS"), TEXT("BaseBoardProduct"), /*out*/ Result);
#endif
    return Result;
}

bool UAdvanceGameToolLibrary::SetActiveMonitor(int32 MonitorIndex)
{
    if (MonitorIndex > GetMonitorsCount() - 1) return false;

    if (!GEngine) return false;

    if (!GEngine->GameViewport) return false;

    FDisplayMetrics Display;
    FDisplayMetrics::RebuildDisplayMetrics(Display);

    if (!Display.MonitorInfo.IsValidIndex(MonitorIndex)) return false;

    const FMonitorInfo Monitor = Display.MonitorInfo[MonitorIndex];

    // Check if used monitor resolution is bigger then selected new monitor resolution if so change game resolution to selected monitor resolution
    // check if UserSettings.ResolutionSizeX > Monitor.NativeWidth || UserSettings.ResolutionSizeY > Monitor.NativeHeight
    // if true then change your game resolution to Monitor.NativeWidth, Monitor.NativeHeight

    const int32 WindowPosX = Monitor.WorkArea.Left;
    const int32 WindowPosY = Monitor.WorkArea.Top;
    const FVector2D Position(static_cast<float>(WindowPosX), static_cast<float>(WindowPosY));

    TSharedPtr<SWindow> Window = GEngine->GameViewport->GetWindow();

    // Best to set window position in WindowMode for Linux
#if PLATFORM_LINUX
    const EWindowMode::Type CurrentWindowMode = Window->GetWindowMode();
    Window->SetWindowMode(EWindowMode::Windowed);
#endif

    Window->MoveWindowTo(Position);

#if PLATFORM_LINUX
    // Set WindowMode back to mode before MoveWindow
    Window->SetWindowMode(CurrentWindowMode);
#endif

    return true;
}

void UAdvanceGameToolLibrary::SetGamma(float Gamma)
{
    if (GEngine)
    {
        GEngine->DisplayGamma = FMath::Clamp(Gamma, 1.0f, 5.0f);
    }
}

float UAdvanceGameToolLibrary::GetGamma()
{
    return GEngine ? GEngine->GetDisplayGamma() : -1.0f;
}

FString UAdvanceGameToolLibrary::GetProjectVersion()
{
    return GetDefault<UGeneralProjectSettings>()->ProjectVersion;
}

FString UAdvanceGameToolLibrary::GetProjectName()
{
    return GetDefault<UGeneralProjectSettings>()->ProjectName;
}

FString UAdvanceGameToolLibrary::GetCompanyName()
{
    return GetDefault<UGeneralProjectSettings>()->CompanyName;
}

FString UAdvanceGameToolLibrary::GetCopyrightNotice()
{
    return GetDefault<UGeneralProjectSettings>()->CopyrightNotice;
}

#pragma endregion
