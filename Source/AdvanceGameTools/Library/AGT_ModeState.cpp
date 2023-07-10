/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"

#pragma region ModeState

bool UAdvanceGameToolLibrary::IsEditor()
{
#if WITH_EDITOR
    return true;
#endif

    return false;
}

bool UAdvanceGameToolLibrary::IsBuildDebug()
{
#if UE_BUILD_DEBUG
    return true;
#endif

    return false;
}

bool UAdvanceGameToolLibrary::IsBuildDevelopment()
{
#if UE_BUILD_DEVELOPMENT
    return true;
#endif
    return false;
}

bool UAdvanceGameToolLibrary::IsBuildShipping()
{
#if UE_BUILD_SHIPPING
    return true;
#endif
    return false;
}

void UAdvanceGameToolLibrary::SwitchPlatform(EPlatformType& PlatformType)
{
    PlatformType = EPlatformType::None;

#if PLATFORM_WINDOWS
    PlatformType = EPlatformType::Widows;
#endif

#if PLATFORM_LINUX
    PlatformType = EPlatformType::Linux;
#endif

#if PLATFORM_MAC
    PlatformType = EPlatformType::Mac;
#endif

#if PLATFORM_HOLOLENS
    PlatformType = EPlatformType::Hololens;
#endif

#if PLATFORM_IOS
    PlatformType = EPlatformType::IOS;
#endif

#if PLATFORM_SWITCH
    PlatformType = EPlatformType::Switch;
#endif
}

FString UAdvanceGameToolLibrary::GetNetModeToString(AActor* InActor)
{
    if (!InActor) return FString("NM_Client");

    const ENetMode NetMode = InActor->GetNetMode();

    switch (NetMode)
    {
        case NM_Standalone: return FString("NM_Standalone");
        case NM_DedicatedServer: return FString("NM_DedicatedServer");
        case NM_Client: return FString("NM_Client");
        case NM_ListenServer: return FString("NM_ListenServer");
    }
    return FString("nullptr");
}

#pragma endregion
