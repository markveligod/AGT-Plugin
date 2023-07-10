/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "Misc/FileHelper.h"

#pragma region ActionFiles

bool UAdvanceGameToolLibrary::FileSaveString(FString Text, FString FileName)
{
    return FFileHelper::SaveStringToFile(Text, *(FPaths::ProjectLogDir() + FileName), FFileHelper::EEncodingOptions::ForceUTF8);
}

bool UAdvanceGameToolLibrary::FileLoadString(FString FileName, FString& Text)
{
    return FFileHelper::LoadFileToString(Text, *(FPaths::ProjectLogDir() + FileName));
}

#pragma endregion