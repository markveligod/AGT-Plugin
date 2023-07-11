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

UAGTFileHandle* UAdvanceGameToolLibrary::OpenFileHandle(UObject* outer, const FString filePath, const bool forRead, const bool forWrite, bool& success)
{
    IFileHandle* fileHandle = nullptr;
    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    if(forRead)
    {
        fileHandle = platformFile.OpenRead(*filePath, forWrite);
    }
    else if(forWrite)
    {
        fileHandle = platformFile.OpenWrite(*filePath, false, forRead);
    }
	
    UAGTFileHandle* handle = nullptr;
    if(fileHandle)
    {
        handle = NewObject<UAGTFileHandle>(outer);
        if(handle)
        {
            handle->Handle = fileHandle;
            handle->CanRead = forRead;
            handle->CanWrite = forWrite;
            success = true;
        }
    }

    return handle;
}

bool UAdvanceGameToolLibrary::WriteBytesToFile(const FString filePath, const bool allowOverwrite, const TArray<uint8>& bytesOut)
{
    if(FPaths::FileExists(filePath) && !allowOverwrite)
    {
        return false;
    }

    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenWrite(*filePath);
    if(fileHandle)
    {
        const bool writeSuccess = fileHandle->Write(bytesOut.GetData(), bytesOut.Num());
        delete fileHandle;
        return writeSuccess;
    }
	
    return false;
}

bool UAdvanceGameToolLibrary::ReadAllBytesFromFile(const FString filePath, TArray<uint8>& bytesIn)
{
    if(!FPaths::FileExists(filePath))
    {
        return false;
    }

    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenRead(*filePath);
    if(fileHandle)
    {
        bytesIn.SetNum(fileHandle->Size());
        const bool readSuccess = fileHandle->Read(bytesIn.GetData(), bytesIn.Num());
        delete fileHandle;
        return readSuccess;
    }
	
    return false;
}

bool UAdvanceGameToolLibrary::ReadBytesFromFile(const FString filePath, TArray<uint8>& bytesIn, const int64 offset, const int64 numBytes)
{
    if(!FPaths::FileExists(filePath))
    {
        return false;
    }

    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenRead(*filePath);
    if(fileHandle)
    {
        if(!fileHandle->Seek(offset))
        {
            delete fileHandle;
            return false;
        }
		
        const int64 numToRead = FMath::Min(fileHandle->Size() - fileHandle->Tell(), numBytes);
        if(numToRead <= 0)
        {
            delete fileHandle;
            return false;
        }

        bytesIn.SetNum(numToRead);
        const bool readSuccess = fileHandle->Read(bytesIn.GetData(), bytesIn.Num());
        delete fileHandle;
        return readSuccess;
    }
	
    return false;
}

#pragma endregion
