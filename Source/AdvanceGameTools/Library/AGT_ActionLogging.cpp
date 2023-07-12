/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"

#pragma region ActionLogging

void UAdvanceGameToolLibrary::ErrorLog(const FString& Message)
{
    FMessageLog(FName("PIE")).Error(FText::FromString(Message));
}

void UAdvanceGameToolLibrary::WarningLog(const FString& Message)
{
    FMessageLog(FName("PIE")).Warning(FText::FromString(Message));
}

void UAdvanceGameToolLibrary::InfoLog(const FString& Message)
{
    FMessageLog(FName("PIE")).Info(FText::FromString(Message));
}

void UAdvanceGameToolLibrary::ActorErrorLog(AActor* Actor, const FString& Message)
{
    if (!IsValid(Actor)) return;

    ErrorLog(Actor->GetName() + ": " + Message);
}

void UAdvanceGameToolLibrary::ActorWarningLog(AActor* Actor, const FString& Message)
{
    if (!IsValid(Actor)) return;

    WarningLog(Actor->GetName() + ": " + Message);
}

void UAdvanceGameToolLibrary::ActorInfoLog(AActor* Actor, const FString& Message)
{
    if (!IsValid(Actor)) return;

    InfoLog(Actor->GetName() + ": " + Message);
}

void UAdvanceGameToolLibrary::DebugLogError(const FString& Message)
{
#if WITH_EDITOR
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, Message);
#endif
}

void UAdvanceGameToolLibrary::DebugLogWarning(const FString& Message)
{
#if WITH_EDITOR
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, Message);
#endif
}

void UAdvanceGameToolLibrary::DebugLogMessage(const FString& Message)
{
#if WITH_EDITOR
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, Message);
#endif
}

#pragma endregion