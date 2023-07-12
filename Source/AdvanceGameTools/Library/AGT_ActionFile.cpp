/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "ImageUtils.h"
#include "JsonObjectConverter.h"
#include "XmlFile.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Serialization/Csv/CsvParser.h"

class FCustomFileVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
    FString BasePath;
    TArray<FString>& Nodes;
    FString Filter;
    FRegexPattern CustomPattern;
    bool bFile = true;
    bool bDirectory = true;

    FCustomFileVisitor(FString& Path, TArray<FString>& Paths, const FString& Pattern, bool File, bool Directory)
        : BasePath(Path), Nodes(Paths), Filter(Pattern), CustomPattern(Pattern), bFile(File), bDirectory(Directory){};

    virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
    {
        if ((bFile && !bIsDirectory) || (bDirectory && bIsDirectory))
        {
            FString RelativePath = FString(FilenameOrDirectory);
            FPaths::MakePathRelativeTo(RelativePath, *BasePath);
            if (!Filter.IsEmpty())
            {
                FRegexMatcher CustomMatcher(CustomPattern, RelativePath);
                if (CustomMatcher.FindNext())
                {
                    Nodes.Add(RelativePath);
                }
            }
            else
            {
                Nodes.Add(RelativePath);
            }
        }
        return true;
    }
};

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
    if (forRead)
    {
        fileHandle = platformFile.OpenRead(*filePath, forWrite);
    }
    else if (forWrite)
    {
        fileHandle = platformFile.OpenWrite(*filePath, false, forRead);
    }

    UAGTFileHandle* handle = nullptr;
    if (fileHandle)
    {
        handle = NewObject<UAGTFileHandle>(outer);
        if (handle)
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
    if (FPaths::FileExists(filePath) && !allowOverwrite)
    {
        return false;
    }

    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenWrite(*filePath);
    if (fileHandle)
    {
        const bool writeSuccess = fileHandle->Write(bytesOut.GetData(), bytesOut.Num());
        delete fileHandle;
        return writeSuccess;
    }

    return false;
}

bool UAdvanceGameToolLibrary::ReadAllBytesFromFile(const FString filePath, TArray<uint8>& bytesIn)
{
    if (!FPaths::FileExists(filePath))
    {
        return false;
    }

    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenRead(*filePath);
    if (fileHandle)
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
    if (!FPaths::FileExists(filePath))
    {
        return false;
    }

    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenRead(*filePath);
    if (fileHandle)
    {
        if (!fileHandle->Seek(offset))
        {
            delete fileHandle;
            return false;
        }

        const int64 numToRead = FMath::Min(fileHandle->Size() - fileHandle->Tell(), numBytes);
        if (numToRead <= 0)
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

#pragma region Paths

FEnginePath UAdvanceGameToolLibrary::GetEngineDirectories()
{
    FEnginePath P;
    P.Directory = FPaths::EngineDir();
    P.Config = FPaths::EngineConfigDir();
    P.Content = FPaths::EngineContentDir();
    P.Intermediate = FPaths::EngineIntermediateDir();
    P.Plugins = FPaths::EnginePluginsDir();
    P.Saved = FPaths::EngineSavedDir();
    P.User = FPaths::EngineUserDir();
    P.DefaultLayout = FPaths::EngineDefaultLayoutDir();
    P.PlatformExtensions = FPaths::EnginePlatformExtensionsDir();
    P.UserLayout = FPaths::EngineUserLayoutDir();
    return P;
}

FProjectPath UAdvanceGameToolLibrary::GetProjectDirectories()
{
    FProjectPath P;
    P.Directory = FPaths::ProjectDir();
    P.Config = FPaths::ProjectConfigDir();
    P.Content = FPaths::ProjectContentDir();
    P.Intermediate = FPaths::ProjectIntermediateDir();
    P.Log = FPaths::ProjectLogDir();
    P.Mods = FPaths::ProjectModsDir();
    P.Plugins = FPaths::ProjectPluginsDir();
    P.Saved = FPaths::ProjectSavedDir();
    P.User = FPaths::ProjectUserDir();
    P.PersistentDownload = FPaths::ProjectPersistentDownloadDir();
    P.PlatformExtensions = FPaths::ProjectPlatformExtensionsDir();
    return P;
}

#pragma endregion

#pragma region TextFile

bool UAdvanceGameToolLibrary::ReadText(FString Path, FString& Output)
{
    IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
    if (file.FileExists(*Path))
    {
        return FFileHelper::LoadFileToString(Output, *Path);
    }
    return false;
}

bool UAdvanceGameToolLibrary::SaveText(FString Path, FString Text, FString& Error, bool Append, bool Force)
{
    IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
    FText ErrorFilename;
    if (!FFileHelper::IsFilenameValidForSaving(Path, ErrorFilename))
    {
        Error = FString("Filename is not valid");
        return false;
    }
    if (!file.FileExists(*Path) || Append || Force)
    {
        return FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), Append ? FILEWRITE_Append : FILEWRITE_None);
    }
    else
    {
        Error = FString("File already exists");
    }
    return false;
}

bool UAdvanceGameToolLibrary::ReadLine(FString Path, FString Pattern, TArray<FString>& Lines)
{
    IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
    if (!file.FileExists(*Path))
    {
        return false;
    }
    if (!Pattern.IsEmpty())
    {
        FRegexPattern CustomPattern(Pattern);
        return FFileHelper::LoadFileToStringArrayWithPredicate(Lines, *Path,
            [CustomPattern](const FString Line)
            {
                FRegexMatcher CustomMatcher(CustomPattern, Line);
                return CustomMatcher.FindNext();
            });
    }
    return FFileHelper::LoadFileToStringArray(Lines, *Path);
}

bool UAdvanceGameToolLibrary::SaveLine(FString Path, const TArray<FString>& Text, FString& Error, bool Append, bool Force)
{
    IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
    FText ErrorFilename;
    if (!FFileHelper::IsFilenameValidForSaving(Path, ErrorFilename))
    {
        Error = FString("Filename is not valid");
        return false;
    }
    if (!file.FileExists(*Path) || Append || Force)
    {
        return FFileHelper::SaveStringArrayToFile(Text, *Path, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), Append ? FILEWRITE_Append : FILEWRITE_None);
    }
    Error = FString("File already exists");
    return false;
}

bool UAdvanceGameToolLibrary::ReadByte(FString Path, TArray<uint8>& Bytes)
{
    IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
    if (!file.FileExists(*Path))
    {
        return false;
    }
    return FFileHelper::LoadFileToArray(Bytes, *Path);
}

bool UAdvanceGameToolLibrary::SaveByte(FString Path, const TArray<uint8>& Bytes, FString& Error, bool Append, bool Force)
{
    IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
    FText ErrorFilename;
    if (!FFileHelper::IsFilenameValidForSaving(Path, ErrorFilename))
    {
        Error = FString("Filename is not valid");
        return false;
    }
    if (!file.FileExists(*Path) || Append || Force)
    {
        return FFileHelper::SaveArrayToFile(Bytes, *Path, &IFileManager::Get(), Append ? FILEWRITE_Append : FILEWRITE_None);
    }
    Error = FString("File already exists");
    return false;
}

#pragma endregion

#pragma region Base64

FString UAdvanceGameToolLibrary::StringToBase64(const FString Source)
{
    return FBase64::Encode(Source);
}

bool UAdvanceGameToolLibrary::StringFromBase64(FString Base64Str, FString& Result)
{
    return FBase64::Decode(Base64Str, Result);
}

FString UAdvanceGameToolLibrary::BytesToBase64(const TArray<uint8> Bytes)
{
    return FBase64::Encode(Bytes);
}

bool UAdvanceGameToolLibrary::BytesFromBase64(const FString Source, TArray<uint8>& Out)
{
    return FBase64::Decode(Source, Out);
}

#pragma endregion

#pragma region CSVFile

bool UAdvanceGameToolLibrary::SaveCSV(FString Path, TArray<FString> Headers, TArray<FString> Data, int32& Total, bool Force)
{
    FString Output;
    if (!UAdvanceGameToolLibrary::CSVToString(Output, Headers, Data, Total))
    {
        return false;
    }
    FString Error;
    return UAdvanceGameToolLibrary::SaveText(Path, Output, Error, false, Force);
}

bool UAdvanceGameToolLibrary::ReadCSV(FString Path, TArray<FString>& Headers, TArray<FString>& Data, int32& Total, bool HeaderFirst)
{
    Total = 0;
    IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
    if (!file.FileExists(*Path))
    {
        return false;
    }
    FString Result;
    if (!FFileHelper::LoadFileToString(Result, *Path))
    {
        return false;
    }
    return UAdvanceGameToolLibrary::StringToCSV(Result, Headers, Data, Total, HeaderFirst);
}

#pragma endregion

#pragma region CSVConvert

bool UAdvanceGameToolLibrary::StringToCSV(FString Content, TArray<FString>& Headers, TArray<FString>& Data, int32& Total, bool HeaderFirst)
{
    FCsvParser Parser(Content);
    TArray<TArray<const TCHAR*>> Rows = Parser.GetRows();
    for (TArray<const TCHAR*> Row : Rows)
    {
        Total++;
        for (FString Col : Row)
        {
            if (Total == 1 && HeaderFirst)
            {
                Headers.Add(Col);
            }
            else
            {
                Data.Add(Col);
            }
        }
    }
    return true;
}

bool UAdvanceGameToolLibrary::CSVToString(FString& Result, TArray<FString> Headers, TArray<FString> Data, int32& Total)
{
    Total = 0;
    FString Delimiter = ",";
    if (Headers.Num() == 0)
    {
        return false;
    }
    if (Data.Num() % Headers.Num() != 0)
    {
        return false;
    }
    Result = TEXT("");
    // Header row
    for (FString Col : Headers)
    {
        if (Result.Len() > 0)
        {
            Result += Delimiter;
        }
        Result += (TEXT("\"") + Col.Replace(TEXT("\""), TEXT("\"\"")) + TEXT("\""));
    }
    Result += LINE_TERMINATOR;
    FString Row = TEXT("");
    int32 Count = 0;
    // Data row
    for (FString Col : Data)
    {
        Count++;
        if (Row.Len() > 0)
        {
            Row += Delimiter;
        }
        Row += (TEXT("\"") + Col.Replace(TEXT("\""), TEXT("\"\"")) + TEXT("\""));
        if (Count % Headers.Num() == 0)
        {
            Row += LINE_TERMINATOR;
            Result += Row;
            Row = "";
        }
    }
    Total = (Data.Num() / Headers.Num()) + 1;
    return true;
}

#pragma endregion

#pragma region XMLFile

bool UAdvanceGameToolLibrary::AnyStructToXmlString(FProperty* Property, void* ValuePtr, FString& Xml)
{
    if (!Property || ValuePtr == NULL)
    {
        return false;
    }
    auto JsonValue = UAdvanceGameToolLibrary::AnyStructToJsonValue(Property, ValuePtr);
    Xml = UAdvanceGameToolLibrary::AnyStructToXmlValue(Property, JsonValue, 1);
    UAdvanceGameToolLibrary::CreateTagNode("root", Xml, 0, true);
    Xml = "<?xml version='1.0' encoding='UTF-8' ?>" + Xml;
    return true;
}

FString UAdvanceGameToolLibrary::AnyStructToXmlValue(FProperty* Property, TSharedPtr<FJsonValue> Value, int32 Depth)
{
    FString Out;
    if (!Property || Value == NULL)
    {
        return "";
    }
    else if (FArrayProperty* arrayProperty = CastField<FArrayProperty>(Property))
    {
        if (Value->Type == EJson::Array)
        {
            auto Array = Value->AsArray();
            FString Arr;
            for (int32 ArrayIndex = 0; ArrayIndex < Array.Num(); ArrayIndex++)
            {
                auto Val = Array[ArrayIndex];
                Arr = UAdvanceGameToolLibrary::AnyStructToXmlValue(arrayProperty->Inner, Val, Depth + 1);
                UAdvanceGameToolLibrary::CreateTagNode("value", Arr, Depth, (Val->Type == EJson::Array || Val->Type == EJson::Object));
                Out += Arr;
            }
        }
        else
        {
            // error
            return "";
        }
    }
    else if (FSetProperty* setProperty = CastField<FSetProperty>(Property))
    {
        if (Value->Type == EJson::Array)
        {
            auto Array = Value->AsArray();
            FString Arr;
            for (int32 ArrayIndex = 0; ArrayIndex < Array.Num(); ArrayIndex++)
            {
                auto Val = Array[ArrayIndex];
                Arr = UAdvanceGameToolLibrary::AnyStructToXmlValue(setProperty->ElementProp, Val, Depth + 1);
                UAdvanceGameToolLibrary::CreateTagNode("item", Arr, Depth, (Val->Type == EJson::Array || Val->Type == EJson::Object));
                Out += Arr;
            }
        }
        else
        {
            // error
            return "";
        }
    }
    // map
    else if (FMapProperty* mapProperty = CastField<FMapProperty>(Property))
    {
        if (Value->Type == EJson::Object)
        {
            auto Object = Value->AsObject()->Values;
            FString Arr;
            for (auto KeyVal : Object)
            {
                FString Item;
                Arr = KeyVal.Key;
                UAdvanceGameToolLibrary::CreateTagNode("key", Arr, Depth + 1, false);
                Item += Arr;
                Arr = UAdvanceGameToolLibrary::AnyStructToXmlValue(mapProperty->ValueProp, KeyVal.Value, Depth + 2);
                UAdvanceGameToolLibrary::CreateTagNode("value", Arr, Depth + 1, (KeyVal.Value->Type == EJson::Array || KeyVal.Value->Type == EJson::Object));
                Item += Arr;
                UAdvanceGameToolLibrary::CreateTagNode("item", Item, Depth, true);
                Out += Item;
            }
        }
        else
        {
            // error
            return "";
        }
    }
    // struct
    else if (FStructProperty* structProperty = CastField<FStructProperty>(Property))
    {
        if (Value->Type == EJson::Object)
        {
            auto Object = Value->AsObject();
            FString Arr;
            for (TFieldIterator<FProperty> It(structProperty->Struct); It; ++It)
            {
                FProperty* Prop = *It;
                if (Object->HasField(Prop->GetAuthoredName()))
                {
                    auto Val = Object->Values.Find(Prop->GetAuthoredName())->ToSharedRef();
                    Arr = UAdvanceGameToolLibrary::AnyStructToXmlValue(Prop, Val, Depth + 1);
                    UAdvanceGameToolLibrary::CreateTagNode(Prop->GetAuthoredName(), Arr, Depth, (Val->Type == EJson::Array || Val->Type == EJson::Object));
                    Out += Arr;
                }
                else if (Object->HasField(Prop->GetName()))
                {
                    auto Val = Object->Values.Find(Prop->GetName())->ToSharedRef();
                    Arr = UAdvanceGameToolLibrary::AnyStructToXmlValue(Prop, Val, Depth + 1);
                    UAdvanceGameToolLibrary::CreateTagNode(Prop->GetName(), Arr, Depth, (Val->Type == EJson::Array || Val->Type == EJson::Object));
                    Out += Arr;
                }
            }
        }
        else
        {
            // error
            return "";
        }
    }
    // object
    else if (FObjectProperty* objectProperty = CastField<FObjectProperty>(Property))
    {
        FString Arr;
        if (objectProperty->PropertyClass->IsNative())
        {
            if (Value->Type == EJson::String)
            {
                Out = Value->AsString();
            }
            else
            {
                WarningLog(FString::Printf(TEXT("AnyStructToXmlString : Error property %s type %i error"), *objectProperty->GetAuthoredName(), Value->Type));
            }
        }
        else if (Value->Type == EJson::Object)
        {
            auto Object = Value->AsObject();
            for (TFieldIterator<FProperty> It(objectProperty->PropertyClass); It; ++It)
            {
                FProperty* Prop = *It;
                if (Object->HasField(Prop->GetAuthoredName()))
                {
                    auto Val = Object->Values.Find(Prop->GetAuthoredName())->ToSharedRef();
                    Arr = UAdvanceGameToolLibrary::AnyStructToXmlValue(Prop, Val, Depth + 1);
                    UAdvanceGameToolLibrary::CreateTagNode(Prop->GetAuthoredName(), Arr, Depth, (Val->Type == EJson::Array || Val->Type == EJson::Object));
                    Out += Arr;
                }
                else if (Object->HasField(Prop->GetName()))
                {
                    auto Val = Object->Values.Find(Prop->GetName())->ToSharedRef();
                    Arr = UAdvanceGameToolLibrary::AnyStructToXmlValue(Prop, Val, Depth + 1);
                    UAdvanceGameToolLibrary::CreateTagNode(Prop->GetName(), Arr, Depth, (Val->Type == EJson::Array || Val->Type == EJson::Object));
                    Out += Arr;
                }
            }
        }
        else
        {
            // error
            return "";
        }
    }
    // scalar
    else
    {
        switch (Value->Type)
        {
            case (EJson::String): Out = UAdvanceGameToolLibrary::XmlEscapeChars(Value->AsString()); break;
            case (EJson::Number): Out = UAdvanceGameToolLibrary::XmlEscapeChars(FString::SanitizeFloat(Value->AsNumber())); break;
            case (EJson::Boolean): Out = UAdvanceGameToolLibrary::XmlEscapeChars((Value->AsBool() ? "true" : "false")); break;
            case (EJson::Null): Out = ""; break;
        }
    }
    return Out;
}

FString& UAdvanceGameToolLibrary::CreateTagNode(FString Tag, FString& Content, int32 Depth, bool WithCR)
{
    FString Tab;
    for (int32 space = 0; space < (Depth * 2); space++)
    {
        Tab += " ";
    }
    Content = LINE_TERMINATOR + Tab + "<" + Tag + ">" + Content;
    if (WithCR)
    {
        Content += LINE_TERMINATOR + Tab;
    }
    Content += "</" + Tag + ">";
    return Content;
}

bool UAdvanceGameToolLibrary::XmlStringToAnyStruct(FProperty* Property, void* ValuePtr, const FString& Xml)
{
    bool Success = false;
    if (!Property || !ValuePtr || Xml.IsEmpty())
    {
        return Success;
    }
    FXmlFile* File = new FXmlFile();
    if (!File->LoadFile(Xml, EConstructMethod::ConstructFromBuffer))
    {
        WarningLog(FString::Printf(TEXT("XmlStringToAnyStruct : Could not load xml buffer %s"), *(File->GetLastError())));
        File->Clear();
        return Success;
    }
    auto JsonValue = UAdvanceGameToolLibrary::XmlNodeToAnyStruct(Property, File->GetRootNode());
    if ((JsonValue->Type == EJson::Array && JsonValue->AsArray().Num() != 0) || (JsonValue->Type == EJson::Object && JsonValue->AsObject()->Values.Num() != 0))
    {
        Success = FJsonObjectConverter::JsonValueToUProperty(JsonValue, Property, ValuePtr, 0, 0);
    }
    File->Clear();
    delete File;
    return Success;
}

TSharedRef<FJsonValue> UAdvanceGameToolLibrary::XmlNodeToAnyStruct(FProperty* Property, FXmlNode* Node)
{
    if (!Property || !Node)
    {
        return MakeShareable(new FJsonValueNull());
    }
    else if (FArrayProperty* arrayProperty = CastField<FArrayProperty>(Property))
    {
        TArray<TSharedPtr<FJsonValue>> Array;
        auto Children = Node->GetChildrenNodes();
        for (int ArrayIndex = 0; ArrayIndex < Children.Num(); ArrayIndex++)
        {
            Array.Add(UAdvanceGameToolLibrary::XmlNodeToAnyStruct(arrayProperty->Inner, Children[ArrayIndex]));
        }
        return MakeShareable(new FJsonValueArray(Array));
    }
    else if (FSetProperty* setProperty = CastField<FSetProperty>(Property))
    {
        TArray<TSharedPtr<FJsonValue>> Array;
        auto Children = Node->GetChildrenNodes();
        for (int ArrayIndex = 0; ArrayIndex < Children.Num(); ArrayIndex++)
        {
            Array.Add(UAdvanceGameToolLibrary::XmlNodeToAnyStruct(setProperty->ElementProp, Children[ArrayIndex]));
        }
        return MakeShareable(new FJsonValueArray(Array));
    }
    else if (FMapProperty* mapProperty = CastField<FMapProperty>(Property))
    {
        TSharedRef<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
        for (auto Child : Node->GetChildrenNodes())
        {
            if (auto Key = Child->FindChildNode("key"))
            {
                if (auto Val = Child->FindChildNode("value"))
                {
                    Object->SetField(Key->GetContent(), UAdvanceGameToolLibrary::XmlNodeToAnyStruct(mapProperty->ValueProp, Val));
                }
            }
        }
        return MakeShareable(new FJsonValueObject(Object));
    }
    else if (FStructProperty* structProperty = CastField<FStructProperty>(Property))
    {
        TSharedRef<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
        for (TFieldIterator<FProperty> It(structProperty->Struct); It; ++It)
        {
            FProperty* Prop = *It;
            FXmlNode* Result;
            if ((Result = Node->FindChildNode(Prop->GetAuthoredName())))
            {
                Object->SetField(Prop->GetName(), UAdvanceGameToolLibrary::XmlNodeToAnyStruct(Prop, Result));
            }
            else if ((Result = Node->FindChildNode(Prop->GetName())))
            {
                Object->SetField(Prop->GetName(), UAdvanceGameToolLibrary::XmlNodeToAnyStruct(Prop, Result));
            }
        }
        return MakeShareable(new FJsonValueObject(Object));
    }
    else if (FObjectProperty* objectProperty = CastField<FObjectProperty>(Property))
    {
        if (Node->GetContent().IsEmpty() && Node->GetChildrenNodes().Num() == 0)
        {
            return MakeShareable(new FJsonValueNull());
        }
        if (objectProperty->PropertyClass->IsNative())
        {
            return MakeShareable(new FJsonValueString(Node->GetContent()));
        }
        TSharedRef<FJsonObject> Object = MakeShared<FJsonObject>();
        for (TFieldIterator<FProperty> It(objectProperty->PropertyClass); It; ++It)
        {
            FProperty* Prop = *It;
            FXmlNode* Result;
            if ((Result = Node->FindChildNode(Prop->GetAuthoredName())))
            {
                Object->SetField(Prop->GetName(), UAdvanceGameToolLibrary::XmlNodeToAnyStruct(Prop, Result));
            }
            else if ((Result = Node->FindChildNode(Prop->GetName())))
            {
                Object->SetField(Prop->GetName(), UAdvanceGameToolLibrary::XmlNodeToAnyStruct(Prop, Result));
            }
        }
        return MakeShareable(new FJsonValueObject(Object));
    }
    // scalar
    else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
    {
        auto Prop = MakeShareable<FJsonValueBoolean>(new FJsonValueBoolean((Node->GetContent() == "true" ? true : false)));
        return Prop;
    }
    else if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
    {
        auto Prop = MakeShareable<FJsonValueNumber>(new FJsonValueNumber(FCString::Atod(*Node->GetContent())));
        return Prop;
    }
    else if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
    {
        auto Prop = MakeShareable<FJsonValueString>(new FJsonValueString(UAdvanceGameToolLibrary::XmlConvertChars(Node->GetContent())));
        return Prop;
    }
    // other
    auto Prop = MakeShareable<FJsonValueString>(new FJsonValueString(UAdvanceGameToolLibrary::XmlConvertChars(Node->GetContent())));
    return Prop;
}

FString UAdvanceGameToolLibrary::XmlEscapeChars(FString Source)
{
    Source.ReplaceInline(TEXT("&"), TEXT("&amp;"));
    Source.ReplaceInline(TEXT("<"), TEXT("&lt;"));
    Source.ReplaceInline(TEXT(">"), TEXT("&gt;"));
    return Source;
}

FString UAdvanceGameToolLibrary::XmlConvertChars(FString Source)
{
    Source.ReplaceInline(TEXT("&gt;"), TEXT(">"));
    Source.ReplaceInline(TEXT("&lt;"), TEXT("<"));
    Source.ReplaceInline(TEXT("&amp;"), TEXT("&"));
    return Source;
}

#pragma endregion

#pragma region FileSystem

bool UAdvanceGameToolLibrary::IsFile(FString Path)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    return File.FileExists(*Path);
}

bool UAdvanceGameToolLibrary::IsDirectory(FString Path)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    return File.DirectoryExists(*Path);
}

bool UAdvanceGameToolLibrary::IsValidFilename(FString Filename)
{
    FText Error;
    return FFileHelper::IsFilenameValidForSaving(*Filename, Error);
}

bool UAdvanceGameToolLibrary::ValidateFilename(FString Filename, FString& ValidName)
{
    ValidName = FPaths::MakeValidFileName(Filename);
    return IsValidFilename(ValidName);
}

bool UAdvanceGameToolLibrary::SetReadOnlyFlag(FString FilePath, bool Flag)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    return File.SetReadOnly(*FilePath, Flag);
}

bool UAdvanceGameToolLibrary::GetReadOnlyFlag(FString FilePath)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    return File.IsReadOnly(*FilePath);
}

int64 UAdvanceGameToolLibrary::GetFileSize(FString FilePath)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    return File.FileSize(*FilePath);
}

bool UAdvanceGameToolLibrary::ListDirectory(FString Path, FString Pattern, TArray<FString>& Nodes, bool ShowFile, bool ShowDirectory, bool Recursive)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    if (!File.DirectoryExists(*Path))
    {
        return false;
    }
    if (!ShowDirectory && !ShowFile)
    {
        return true;
    }
    FString BasePath = FPaths::Combine(Path, TEXT("/"));
    FCustomFileVisitor CustomFileVisitor(BasePath, Nodes, Pattern, ShowFile, ShowDirectory);
    if (Recursive)
    {
        return File.IterateDirectoryRecursively(*Path, CustomFileVisitor);
    }
    return File.IterateDirectory(*Path, CustomFileVisitor);
}

bool UAdvanceGameToolLibrary::MakeDirectory(FString Path, bool Recursive)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    if (File.DirectoryExists(*Path))
    {
        return true;
    }
    if (Recursive)
    {
        return File.CreateDirectoryTree(*Path);
    }
    return File.CreateDirectory(*Path);
}

bool UAdvanceGameToolLibrary::RemoveDirectory(FString Path, bool Recursive)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    if (!File.DirectoryExists(*Path))
    {
        return true;
    }
    if (Recursive)
    {
        return File.DeleteDirectoryRecursively(*Path);
    }
    return File.DeleteDirectory(*Path);
}

bool UAdvanceGameToolLibrary::CopyDirectory(FString Source, FString Dest)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    FPaths::NormalizeDirectoryName(Dest);
    if (!File.DirectoryExists(*Source) && !File.FileExists(*Source))
    {
        return false;
    }
    if (!File.DirectoryExists(*Dest))
    {
        return false;
    }
    return File.CopyDirectoryTree(*Dest, *Source, true);
}

bool UAdvanceGameToolLibrary::MoveDirectory(FString Source, FString Dest)
{
    FPaths::NormalizeDirectoryName(Source);
    FPaths::NormalizeDirectoryName(Dest);
    if (Dest.Equals(Source))
    {
        return true;
    }
    if (!UAdvanceGameToolLibrary::CopyDirectory(Source, Dest))
    {
        return false;
    }
    return UAdvanceGameToolLibrary::RemoveDirectory(Source, true);
}

bool UAdvanceGameToolLibrary::NodeStats(FString Path, FCustomNodeStat& Stats)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    if (!File.DirectoryExists(*Path) && !File.FileExists(*Path))
    {
        return false;
    }
    auto Data = File.GetStatData(*Path);
    if (!Data.bIsValid)
    {
        return false;
    }
    Stats.CreationTime = Data.CreationTime;
    Stats.FileSize = Data.FileSize;
    Stats.IsDirectory = Data.bIsDirectory;
    Stats.IsReadOnly = Data.bIsReadOnly;
    Stats.LastAccessTime = Data.AccessTime;
    Stats.ModificationTime = Data.ModificationTime;
    return true;
}

bool UAdvanceGameToolLibrary::RemoveFile(FString Path)
{
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    if (File.DirectoryExists(*Path))
    {
        return false;
    }
    if (!File.FileExists(*Path))
    {
        return true;
    }
    return File.DeleteFile(*Path);
}

bool UAdvanceGameToolLibrary::CopyFile(FString Source, FString Dest, bool Force)
{
    FPaths::NormalizeFilename(Source);
    FPaths::NormalizeFilename(Dest);
    if (Dest.Equals(Source))
    {
        return true;
    }
    FText Error;
    if (!FFileHelper::IsFilenameValidForSaving(*Dest, Error))
    {
        return false;
    }
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    if (!File.FileExists(*Source))
    {
        return false;
    }
    if (!Force && File.FileExists(*Dest))
    {
        return false;
    }
    UAdvanceGameToolLibrary::RemoveFile(Dest);
    return File.CopyFile(*Dest, *Source);
}

bool UAdvanceGameToolLibrary::MoveFile(FString Source, FString Dest, bool Force)
{
    FPaths::NormalizeFilename(Source);
    FPaths::NormalizeFilename(Dest);
    if (Dest.Equals(Source))
    {
        return true;
    }
    FText Error;
    if (!FFileHelper::IsFilenameValidForSaving(*Dest, Error))
    {
        return false;
    }
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    if (!File.FileExists(*Source))
    {
        return false;
    }
    if (!Force && File.FileExists(*Dest))
    {
        return false;
    }
    UAdvanceGameToolLibrary::RemoveFile(Dest);
    return File.MoveFile(*Dest, *Source);
}

bool UAdvanceGameToolLibrary::RenameFile(FString Path, FString NewName)
{
    FText Error;
    if (!FFileHelper::IsFilenameValidForSaving(*NewName, Error))
    {
        return false;
    }
    IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
    if (!File.FileExists(*Path))
    {
        return false;
    }
    FString Clean = FPaths::GetCleanFilename(NewName);
    FString Base = FPaths::GetPath(Path);
    FString Output = FPaths::Combine(*Base, *Clean);
    if (File.FileExists(*Output) || File.DirectoryExists(*Output))
    {
        return false;
    }
    return File.MoveFile(*Output, *Path);
}

void UAdvanceGameToolLibrary::GetPathParts(FString Path, FString& PathPart, FString& BasePart, FString& ExtensionPart, FString& FileName)
{
    PathPart = FPaths::GetPath(Path);
    BasePart = FPaths::GetBaseFilename(Path);
    ExtensionPart = FPaths::GetExtension(Path);
    FileName = FPaths::GetCleanFilename(Path);
}

#pragma endregion

#pragma region Screenshot

bool UAdvanceGameToolLibrary::TakeScreenShot(FString Filename, FString& Path, bool PrefixTimestamp, bool ShowUI)
{
    FText ErrorFilename;
    Filename = FPaths::GetCleanFilename(Filename);
    if (!FFileHelper::IsFilenameValidForSaving(Filename, ErrorFilename))
    {
        return false;
    }
    FString FinalFilename = (PrefixTimestamp ? (FDateTime::Now().ToString(TEXT("%Y_%m_%d__%H_%M_%S__"))) : "") + Filename;
    FScreenshotRequest::Reset();
    FScreenshotRequest::RequestScreenshot(FinalFilename, ShowUI, false);
    if (FScreenshotRequest::IsScreenshotRequested())
    {
        Path = FScreenshotRequest::GetFilename();
        return true;
    }
    return false;
}

UTexture2D* UAdvanceGameToolLibrary::LoadScreenshot(FString Path, bool& Success)
{
    IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
    if (file.FileExists(*Path))
    {
        Success = true;
        UTexture2D* Tex = FImageUtils::ImportFileAsTexture2D(Path);
        if (Tex)
        {
            return Tex;
        }
    }
    Success = false;
    return nullptr;
}

#pragma endregion

#pragma region

bool UAdvanceGameToolLibrary::DatatableToCSV(UDataTable* Table, FString& Output)
{
    if (Table == nullptr || !Table->RowStruct)
    {
        return false;
    }
    return UAdvanceGameToolLibrary::WriteTableToCSV(*Table, Output);
}

bool UAdvanceGameToolLibrary::DataTableToJSON(UDataTable* Table, FString& Output)
{
    if (Table == nullptr || !Table->RowStruct)
    {
        return false;
    }
    return UAdvanceGameToolLibrary::WriteTableToJSON(*Table, Output);
}

UDataTable* UAdvanceGameToolLibrary::CSVToDataTable(FString CSV, UScriptStruct* Struct, bool& Success)
{
    Success = false;
    if (Struct == nullptr)
    {
        return nullptr;
    }
    UDataTable* DataTable = NewObject<UDataTable>();
    DataTable->RowStruct = Struct;
    auto Result = DataTable->CreateTableFromCSVString(CSV);
    if (Result.Num() == 0)
    {
        Success = true;
    }
    return DataTable;
}

UDataTable* UAdvanceGameToolLibrary::JSONToDataTable(FString JSON, UScriptStruct* Struct, bool& Success)
{
    Success = false;
    if (Struct == nullptr)
    {
        return nullptr;
    }
    UDataTable* DataTable = NewObject<UDataTable>();
    DataTable->RowStruct = Struct;
    auto Result = DataTable->CreateTableFromJSONString(JSON);
    if (Result.Num() == 0)
    {
        Success = true;
    }
    return DataTable;
}

bool UAdvanceGameToolLibrary::WriteTableToCSV(const UDataTable& InDataTable, FString& Output)
{
    if (!InDataTable.RowStruct)
    {
        return false;
    }

    // Write the header (column titles)
    FString ImportKeyField;
    if (!InDataTable.ImportKeyField.IsEmpty())
    {
        // Write actual name if we have it
        ImportKeyField = InDataTable.ImportKeyField;
        Output += ImportKeyField;
    }
    else
    {
        Output += TEXT("---");
    }

    FProperty* SkipProperty = nullptr;
    for (TFieldIterator<FProperty> It(InDataTable.RowStruct); It; ++It)
    {
        FProperty* BaseProp = *It;
        check(BaseProp);

        FString ColumnHeader = DataTableUtils::GetPropertyExportName(BaseProp, EDataTableExportFlags::None);

        if (ColumnHeader == ImportKeyField)
        {
            // Don't write header again if this is the name field, and save for skipping later
            SkipProperty = BaseProp;
            continue;
        }

        Output += TEXT(",");
        Output += ColumnHeader;
    }
    Output += TEXT("\n");

    // Write each row
    for (auto RowIt = InDataTable.GetRowMap().CreateConstIterator(); RowIt; ++RowIt)
    {
        FName RowName = RowIt.Key();
        Output += RowName.ToString();

        uint8* RowData = RowIt.Value();
        UAdvanceGameToolLibrary::WriteRowToCSV(InDataTable.RowStruct, RowData, Output);

        Output += TEXT("\n");
    }

    return true;
}

bool UAdvanceGameToolLibrary::WriteRowToCSV(const UScriptStruct* InRowStruct, const void* InRowData, FString& ExportedText)
{
    if (!InRowStruct)
    {
        return false;
    }

    for (TFieldIterator<FProperty> It(InRowStruct); It; ++It)
    {
        FProperty* BaseProp = *It;
        check(BaseProp);

        const void* Data = BaseProp->ContainerPtrToValuePtr<void>(InRowData, 0);
        UAdvanceGameToolLibrary::WriteStructEntryToCSV(InRowData, BaseProp, Data, ExportedText);
    }

    return true;
}

bool UAdvanceGameToolLibrary::WriteStructEntryToCSV(const void* InRowData, FProperty* InProperty, const void* InPropertyData, FString& ExportedText)
{
    ExportedText += TEXT(",");

    const FString PropertyValue = DataTableUtils::GetPropertyValueAsString(InProperty, (uint8*)InRowData, EDataTableExportFlags::None);
    ExportedText += TEXT("\"");
    ExportedText += PropertyValue.Replace(TEXT("\""), TEXT("\"\""));
    ExportedText += TEXT("\"");

    return true;
}

FString UAdvanceGameToolLibrary::GetKeyFieldName(const UDataTable& InDataTable)
{
    FString ExplicitString = InDataTable.ImportKeyField;
    if (ExplicitString.IsEmpty())
    {
        return TEXT("Name");
    }
    return ExplicitString;
}

bool UAdvanceGameToolLibrary::WriteTableToJSON(const UDataTable& InDataTable, FString& OutExportText)
{
    if (!InDataTable.RowStruct)
    {
        return false;
    }

    TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&OutExportText);

    FString KeyField = UAdvanceGameToolLibrary::GetKeyFieldName(InDataTable);

    JsonWriter->WriteArrayStart();

    // Iterate over rows
    for (auto RowIt = InDataTable.GetRowMap().CreateConstIterator(); RowIt; ++RowIt)
    {
        JsonWriter->WriteObjectStart();
        {
            // RowName
            const FName RowName = RowIt.Key();
            JsonWriter->WriteValue(KeyField, RowName.ToString());

            // Now the values
            uint8* RowData = RowIt.Value();
            UAdvanceGameToolLibrary::WriteRowToJSON(InDataTable.RowStruct, RowData, JsonWriter);
        }
        JsonWriter->WriteObjectEnd();
    }

    JsonWriter->WriteArrayEnd();

    JsonWriter->Close();

    return true;
}

bool UAdvanceGameToolLibrary::WriteTableAsObjectToJSON(const UDataTable& InDataTable, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    if (!InDataTable.RowStruct)
    {
        return false;
    }

    JsonWriter->WriteObjectStart(InDataTable.GetName());

    // Iterate over rows
    for (auto RowIt = InDataTable.GetRowMap().CreateConstIterator(); RowIt; ++RowIt)
    {
        // RowName
        const FName RowName = RowIt.Key();
        JsonWriter->WriteObjectStart(RowName.ToString());
        {
            // Now the values
            uint8* RowData = RowIt.Value();
            UAdvanceGameToolLibrary::WriteRowToJSON(InDataTable.RowStruct, RowData, JsonWriter);
        }
        JsonWriter->WriteObjectEnd();
    }
    JsonWriter->WriteObjectEnd();

    return true;
}

bool UAdvanceGameToolLibrary::WriteRowToJSON(const UScriptStruct* InRowStruct, const void* InRowData, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    if (!InRowStruct)
    {
        return false;
    }

    return UAdvanceGameToolLibrary::WriteStructToJSON(InRowStruct, InRowData, JsonWriter);
}

bool UAdvanceGameToolLibrary::WriteStructToJSON(const UScriptStruct* InStruct, const void* InStructData, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    for (TFieldIterator<const FProperty> It(InStruct); It; ++It)
    {
        const FProperty* BaseProp = *It;
        check(BaseProp);

        const FString Identifier = DataTableUtils::GetPropertyExportName(BaseProp, EDataTableExportFlags::UseJsonObjectsForStructs);

        if (BaseProp->ArrayDim == 1)
        {
            const void* Data = BaseProp->ContainerPtrToValuePtr<void>(InStructData, 0);
            UAdvanceGameToolLibrary::WriteStructEntryToJSON(InStructData, BaseProp, Data, JsonWriter);
        }
        else
        {
            JsonWriter->WriteArrayStart(Identifier);

            for (int32 ArrayEntryIndex = 0; ArrayEntryIndex < BaseProp->ArrayDim; ++ArrayEntryIndex)
            {
                const void* Data = BaseProp->ContainerPtrToValuePtr<void>(InStructData, ArrayEntryIndex);
                UAdvanceGameToolLibrary::WriteContainerEntryToJSON(BaseProp, Data, &Identifier, JsonWriter);
            }

            JsonWriter->WriteArrayEnd();
        }
    }

    return true;
}

bool UAdvanceGameToolLibrary::WriteStructEntryToJSON(
    const void* InRowData, const FProperty* InProperty, const void* InPropertyData, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    const FString Identifier = DataTableUtils::GetPropertyExportName(InProperty, EDataTableExportFlags::UseJsonObjectsForStructs);

    if (const FEnumProperty* EnumProp = CastField<const FEnumProperty>(InProperty))
    {
        const FString PropertyValue = DataTableUtils::GetPropertyValueAsString(EnumProp, (uint8*)InRowData, EDataTableExportFlags::UseJsonObjectsForStructs);
        JsonWriter->WriteValue(Identifier, PropertyValue);
    }
    else if (const FNumericProperty* NumProp = CastField<const FNumericProperty>(InProperty))
    {
        if (NumProp->IsEnum())
        {
            const FString PropertyValue = DataTableUtils::GetPropertyValueAsString(InProperty, (uint8*)InRowData, EDataTableExportFlags::UseJsonObjectsForStructs);
            JsonWriter->WriteValue(Identifier, PropertyValue);
        }
        else if (NumProp->IsInteger())
        {
            const int64 PropertyValue = NumProp->GetSignedIntPropertyValue(InPropertyData);
            JsonWriter->WriteValue(Identifier, PropertyValue);
        }
        else
        {
            const double PropertyValue = NumProp->GetFloatingPointPropertyValue(InPropertyData);
            JsonWriter->WriteValue(Identifier, PropertyValue);
        }
    }
    else if (const FBoolProperty* BoolProp = CastField<const FBoolProperty>(InProperty))
    {
        const bool PropertyValue = BoolProp->GetPropertyValue(InPropertyData);
        JsonWriter->WriteValue(Identifier, PropertyValue);
    }
    else if (const FArrayProperty* ArrayProp = CastField<const FArrayProperty>(InProperty))
    {
        JsonWriter->WriteArrayStart(Identifier);

        FScriptArrayHelper ArrayHelper(ArrayProp, InPropertyData);
        for (int32 ArrayEntryIndex = 0; ArrayEntryIndex < ArrayHelper.Num(); ++ArrayEntryIndex)
        {
            const uint8* ArrayEntryData = ArrayHelper.GetRawPtr(ArrayEntryIndex);
            UAdvanceGameToolLibrary::WriteContainerEntryToJSON(ArrayProp->Inner, ArrayEntryData, &Identifier, JsonWriter);
        }

        JsonWriter->WriteArrayEnd();
    }
    else if (const FSetProperty* SetProp = CastField<const FSetProperty>(InProperty))
    {
        JsonWriter->WriteArrayStart(Identifier);

        FScriptSetHelper SetHelper(SetProp, InPropertyData);
        for (int32 SetSparseIndex = 0; SetSparseIndex < SetHelper.GetMaxIndex(); ++SetSparseIndex)
        {
            if (SetHelper.IsValidIndex(SetSparseIndex))
            {
                const uint8* SetEntryData = SetHelper.GetElementPtr(SetSparseIndex);
                UAdvanceGameToolLibrary::WriteContainerEntryToJSON(SetHelper.GetElementProperty(), SetEntryData, &Identifier, JsonWriter);
            }
        }

        JsonWriter->WriteArrayEnd();
    }
    else if (const FMapProperty* MapProp = CastField<const FMapProperty>(InProperty))
    {
        JsonWriter->WriteObjectStart(Identifier);

        FScriptMapHelper MapHelper(MapProp, InPropertyData);
        for (int32 MapSparseIndex = 0; MapSparseIndex < MapHelper.GetMaxIndex(); ++MapSparseIndex)
        {
            if (MapHelper.IsValidIndex(MapSparseIndex))
            {
                const uint8* MapKeyData = MapHelper.GetKeyPtr(MapSparseIndex);
                const uint8* MapValueData = MapHelper.GetValuePtr(MapSparseIndex);

                // JSON object keys must always be strings
                const FString KeyValue = DataTableUtils::GetPropertyValueAsStringDirect(MapHelper.GetKeyProperty(), (uint8*)MapKeyData, EDataTableExportFlags::UseJsonObjectsForStructs);
                UAdvanceGameToolLibrary::WriteContainerEntryToJSON(MapHelper.GetValueProperty(), MapValueData, &KeyValue, JsonWriter);
            }
        }

        JsonWriter->WriteObjectEnd();
    }
    else if (const FStructProperty* StructProp = CastField<const FStructProperty>(InProperty))
    {
        JsonWriter->WriteObjectStart(Identifier);
        UAdvanceGameToolLibrary::WriteStructToJSON(StructProp->Struct, InPropertyData, JsonWriter);
        JsonWriter->WriteObjectEnd();
    }
    else
    {
        const FString PropertyValue = DataTableUtils::GetPropertyValueAsString(InProperty, (uint8*)InRowData, EDataTableExportFlags::UseJsonObjectsForStructs);
        JsonWriter->WriteValue(Identifier, PropertyValue);
    }

    return true;
}

bool UAdvanceGameToolLibrary::WriteContainerEntryToJSON(
    const FProperty* InProperty, const void* InPropertyData, const FString* InIdentifier, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    if (const FEnumProperty* EnumProp = CastField<const FEnumProperty>(InProperty))
    {
        const FString PropertyValue = DataTableUtils::GetPropertyValueAsStringDirect(InProperty, (uint8*)InPropertyData, EDataTableExportFlags::UseJsonObjectsForStructs);
        UAdvanceGameToolLibrary::WriteJSONValueWithOptionalIdentifier(JsonWriter, InIdentifier, *PropertyValue);
    }
    else if (const FNumericProperty* NumProp = CastField<const FNumericProperty>(InProperty))
    {
        if (NumProp->IsEnum())
        {
            const FString PropertyValue = DataTableUtils::GetPropertyValueAsStringDirect(InProperty, (uint8*)InPropertyData, EDataTableExportFlags::UseJsonObjectsForStructs);
            UAdvanceGameToolLibrary::WriteJSONValueWithOptionalIdentifier(JsonWriter, InIdentifier, *PropertyValue);
        }
        else if (NumProp->IsInteger())
        {
            const int64 PropertyValue = NumProp->GetSignedIntPropertyValue(InPropertyData);
            UAdvanceGameToolLibrary::WriteJSONValueWithOptionalIdentifier(JsonWriter, InIdentifier, *FString::FromInt(PropertyValue));
        }
        else
        {
            const double PropertyValue = NumProp->GetFloatingPointPropertyValue(InPropertyData);
            UAdvanceGameToolLibrary::WriteJSONValueWithOptionalIdentifier(JsonWriter, InIdentifier, *FString::SanitizeFloat(PropertyValue));
        }
    }
    else if (const FBoolProperty* BoolProp = CastField<const FBoolProperty>(InProperty))
    {
        const bool PropertyValue = BoolProp->GetPropertyValue(InPropertyData);
        UAdvanceGameToolLibrary::WriteJSONValueWithOptionalIdentifier(JsonWriter, InIdentifier, *(PropertyValue ? FString("true") : FString("false")));
    }
    else if (const FStructProperty* StructProp = CastField<const FStructProperty>(InProperty))
    {
        UAdvanceGameToolLibrary::WriteJSONObjectStartWithOptionalIdentifier(JsonWriter, InIdentifier);
        UAdvanceGameToolLibrary::WriteStructToJSON(StructProp->Struct, InPropertyData, JsonWriter);
        JsonWriter->WriteObjectEnd();
    }
    else if (const FArrayProperty* ArrayProp = CastField<const FArrayProperty>(InProperty))
    {
        // Cannot nest arrays
        return false;
    }
    else if (const FSetProperty* SetProp = CastField<const FSetProperty>(InProperty))
    {
        // Cannot nest sets
        return false;
    }
    else if (const FMapProperty* MapProp = CastField<const FMapProperty>(InProperty))
    {
        // Cannot nest maps
        return false;
    }
    else
    {
        const FString PropertyValue = DataTableUtils::GetPropertyValueAsStringDirect(InProperty, (uint8*)InPropertyData, EDataTableExportFlags::UseJsonObjectsForStructs);
        UAdvanceGameToolLibrary::WriteJSONValueWithOptionalIdentifier(JsonWriter, InIdentifier, *PropertyValue);
    }

    return true;
}

void UAdvanceGameToolLibrary::WriteJSONObjectStartWithOptionalIdentifier(TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter, const FString* InIdentifier)
{
    if (InIdentifier)
    {
        JsonWriter->WriteObjectStart(*InIdentifier);
    }
    else
    {
        JsonWriter->WriteObjectStart();
    }
}

void UAdvanceGameToolLibrary::WriteJSONValueWithOptionalIdentifier(TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter, const FString* InIdentifier, const TCHAR* InValue)
{
    if (InIdentifier)
    {
        JsonWriter->WriteValue(*InIdentifier, InValue);
    }
    else
    {
        JsonWriter->WriteValue(InValue);
    }
}

#pragma endregion

#pragma region ConfigFileINI

bool UAdvanceGameToolLibrary::RemoveConfig(FString FilePath, FString Section, FString Key)
{
    if (!GConfig)
    {
        return false;
    }
    return GConfig->RemoveKey(*Section, *Key, *FilePath);
}

bool UAdvanceGameToolLibrary::WriteConfigFile(FString Filename, FString Section, FString Key, FProperty* Type, void* Value, bool SingleLineArray)
{
    if (!GConfig)
    {
        return false;
    }
    if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Type))
    {
        GConfig->SetBool(*Section, *Key, *(static_cast<bool*>(Value)), Filename);
    }
    else if (FIntProperty* IntProperty = CastField<FIntProperty>(Type))
    {
        GConfig->SetInt(*Section, *Key, *(static_cast<int32*>(Value)), Filename);
    }
    else if (FStrProperty* StrProperty = CastField<FStrProperty>(Type))
    {
        GConfig->SetString(*Section, *Key, **(static_cast<FString*>(Value)), Filename);
    }
    else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Type))
    {
        GConfig->SetFloat(*Section, *Key, *(static_cast<float*>(Value)), Filename);
    }
    else if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Type))
    {
        GConfig->SetDouble(*Section, *Key, *(static_cast<double*>(Value)), Filename);
    }
    else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Type))
    {
        if (FStrProperty* ArrayInnerProperty = CastField<FStrProperty>(ArrayProperty->Inner))
        {
            TArray<FString>* Arr = (static_cast<TArray<FString>*>(Value));
            if (SingleLineArray)
            {
                GConfig->SetSingleLineArray(*Section, *Key, *Arr, Filename);
            }
            else
            {
                GConfig->SetArray(*Section, *Key, *Arr, Filename);
            }
        }
        else
        {
            return false;
        }
    }
    else if (FStructProperty* StructProperty = CastField<FStructProperty>(Type))
    {
        if (StructProperty->Struct->IsNative())
        {
            FName TypeName = StructProperty->Struct->GetFName();
            if (TypeName == "Rotator")
            {
                GConfig->SetRotator(*Section, *Key, *(static_cast<FRotator*>(Value)), *Filename);
            }
            else if (TypeName == "Vector")
            {
                GConfig->SetVector(*Section, *Key, *(static_cast<FVector*>(Value)), *Filename);
            }
            else if (TypeName == "LinearColor")
            {
                GConfig->SetColor(*Section, *Key, *(static_cast<FColor*>(Value)), *Filename);
            }
            else if (TypeName == "Vector4")
            {
                GConfig->SetVector4(*Section, *Key, *(static_cast<FVector4*>(Value)), *Filename);
            }
            else if (TypeName == "Vector2D")
            {
                GConfig->SetVector2D(*Section, *Key, *(static_cast<FVector2D*>(Value)), *Filename);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    GConfig->Flush(false, Filename);
    return true;
}

bool UAdvanceGameToolLibrary::ReadConfigFile(FString Filename, FString Section, FString Key, FProperty* Type, void* Value, bool SingleLineArray)
{
    if (!GConfig)
    {
        return false;
    }
    bool Success = false;
    if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Type))
    {
        Success = GConfig->GetBool(*Section, *Key, *(static_cast<bool*>(Value)), Filename);
    }
    else if (FIntProperty* IntProperty = CastField<FIntProperty>(Type))
    {
        Success = GConfig->GetInt(*Section, *Key, *(static_cast<int32*>(Value)), Filename);
    }
    else if (FStrProperty* StrProperty = CastField<FStrProperty>(Type))
    {
        Success = GConfig->GetString(*Section, *Key, *(static_cast<FString*>(Value)), Filename);
    }
    else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Type))
    {
        Success = GConfig->GetFloat(*Section, *Key, *(static_cast<float*>(Value)), Filename);
    }
    else if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Type))
    {
        Success = GConfig->GetDouble(*Section, *Key, *(static_cast<double*>(Value)), Filename);
    }
    else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Type))
    {
        if (FStrProperty* ArrayInnerProperty = CastField<FStrProperty>(ArrayProperty->Inner))
        {
            TArray<FString>* Arr = (static_cast<TArray<FString>*>(Value));
            if (SingleLineArray)
            {
                Success = (GConfig->GetSingleLineArray(*Section, *Key, *Arr, Filename) != 0);
            }
            else
            {
                Success = (GConfig->GetArray(*Section, *Key, *Arr, Filename) != 0);
            }
        }
    }
    else if (FStructProperty* StructProperty = CastField<FStructProperty>(Type))
    {
        if (StructProperty->Struct->IsNative())
        {
            FName TypeName = StructProperty->Struct->GetFName();
            if (TypeName == "Rotator")
            {
                Success = GConfig->GetRotator(*Section, *Key, *(static_cast<FRotator*>(Value)), *Filename);
            }
            else if (TypeName == "Vector")
            {
                Success = GConfig->GetVector(*Section, *Key, *(static_cast<FVector*>(Value)), *Filename);
            }
            else if (TypeName == "LinearColor")
            {
                Success = GConfig->GetColor(*Section, *Key, *(static_cast<FColor*>(Value)), *Filename);
            }
            else if (TypeName == "Vector4")
            {
                Success = GConfig->GetVector4(*Section, *Key, *(static_cast<FVector4*>(Value)), *Filename);
            }
            else if (TypeName == "Vector2D")
            {
                Success = GConfig->GetVector2D(*Section, *Key, *(static_cast<FVector2D*>(Value)), *Filename);
            }
        }
    }
    return Success;
}

#pragma endregion

#pragma region JSONV2

bool UAdvanceGameToolLibrary::AnyStructToJsonString(FProperty* Property, void* ValuePtr, FString& Json)
{
    bool Success = false;
    if (!Property || ValuePtr == NULL)
    {
        return Success;
    }
    TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Json, 0);
    // convert to json
    auto JsonValue = UAdvanceGameToolLibrary::AnyStructToJsonValue(Property, ValuePtr);
    // serialize
    if (JsonValue->Type == EJson::Object)
    {
        Success = FJsonSerializer::Serialize(JsonValue->AsObject().ToSharedRef(), JsonWriter);
    }
    else if (JsonValue->Type == EJson::Array)
    {
        Success = FJsonSerializer::Serialize(JsonValue->AsArray(), JsonWriter);
    }
    JsonWriter->Close();
    return Success;
}

TSharedRef<FJsonValue> UAdvanceGameToolLibrary::AnyStructToJsonValue(FProperty* Property, void* ValuePtr)
{
    if (ValuePtr == NULL || Property == NULL)
    {
        return MakeShareable(new FJsonValueNull());
    }
    // array
    else if (FArrayProperty* arrayProperty = CastField<FArrayProperty>(Property))
    {
        TArray<TSharedPtr<FJsonValue>> Array;
        auto Helper = FScriptArrayHelper::CreateHelperFormInnerProperty(arrayProperty->Inner, ValuePtr);
        for (int32 ArrayIndex = 0; ArrayIndex < Helper.Num(); ArrayIndex++)
        {
            Array.Add(UAdvanceGameToolLibrary::AnyStructToJsonValue(arrayProperty->Inner, Helper.GetRawPtr(ArrayIndex)));
        }
        return MakeShareable(new FJsonValueArray(Array));
    }
    // set
    else if (FSetProperty* setProperty = CastField<FSetProperty>(Property))
    {
        TArray<TSharedPtr<FJsonValue>> Array;
        auto Helper = FScriptSetHelper::CreateHelperFormElementProperty(setProperty->ElementProp, ValuePtr);
        for (int32 ArrayIndex = 0; ArrayIndex < Helper.Num(); ++ArrayIndex)
        {
            Array.Add(UAdvanceGameToolLibrary::AnyStructToJsonValue(setProperty->ElementProp, Helper.GetElementPtr(ArrayIndex)));
        }
        return MakeShareable(new FJsonValueArray(Array));
    }
    // map
    else if (FMapProperty* mapProperty = CastField<FMapProperty>(Property))
    {
        TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
        auto Helper = FScriptMapHelper::CreateHelperFormInnerProperties(mapProperty->KeyProp, mapProperty->ValueProp, ValuePtr);
        for (int32 ArrayIndex = 0; ArrayIndex < Helper.Num(); ++ArrayIndex)
        {
            FString KeyStr;
            auto Key = UAdvanceGameToolLibrary::AnyStructToJsonValue(mapProperty->KeyProp, Helper.GetKeyPtr(ArrayIndex));
            if (!Key->TryGetString(KeyStr))
            {
                mapProperty->KeyProp->ExportTextItem_InContainer(KeyStr, Helper.GetKeyPtr(ArrayIndex), nullptr, nullptr, 0);
                if (KeyStr.IsEmpty())
                {
                    WarningLog(FString::Printf(TEXT("AnyStructToJsonValue : Error serializing key in map property at index %i, using empty string as key"), ArrayIndex));
                }
            }
            auto Val = UAdvanceGameToolLibrary::AnyStructToJsonValue(mapProperty->ValueProp, Helper.GetValuePtr(ArrayIndex));
            JsonObject->SetField(KeyStr, Val);
        }
        return MakeShareable(new FJsonValueObject(JsonObject));
    }
    // struct
    else if (FStructProperty* structProperty = CastField<FStructProperty>(Property))
    {
        TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
        for (TFieldIterator<FProperty> It(structProperty->Struct); It; ++It)
        {
            FProperty* Prop = *It;
            JsonObject->SetField(Prop->GetAuthoredName(), UAdvanceGameToolLibrary::AnyStructToJsonValue(Prop, Prop->ContainerPtrToValuePtr<void*>(ValuePtr)));
        }
        return MakeShareable(new FJsonValueObject(JsonObject));
    }
    // object
    else if (FObjectProperty* objectProperty = CastField<FObjectProperty>(Property))
    {
        void* PropValue = objectProperty->GetObjectPropertyValue(ValuePtr);
        if (PropValue == NULL)
        {
            return MakeShareable(new FJsonValueNull());
        }
        if (objectProperty->PropertyClass->IsNative())
        {
            auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property, ValuePtr, 0, 0);
            return Value.ToSharedRef();
        }
        TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
        for (TFieldIterator<FProperty> It(objectProperty->PropertyClass); It; ++It)
        {
            FProperty* Prop = *It;
            JsonObject->SetField(Prop->GetAuthoredName(), UAdvanceGameToolLibrary::AnyStructToJsonValue(Prop, Prop->ContainerPtrToValuePtr<void*>(PropValue)));
        }
        return MakeShareable(new FJsonValueObject(JsonObject));
    }

    auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property, ValuePtr, 0, 0);
    return Value.ToSharedRef();
}

bool UAdvanceGameToolLibrary::JsonStringToAnyStruct(FProperty* Property, void* ValuePtr, const FString& Json)
{
    bool Success = false;
    if (!Property || !ValuePtr || Json.IsEmpty())
    {
        return Success;
    }
    TSharedPtr<FJsonValue> JsonValue;
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Json);
    if (FJsonSerializer::Deserialize(JsonReader, JsonValue) && JsonValue.IsValid())
    {
        TSharedRef<FJsonValue> InJsonValue = UAdvanceGameToolLibrary::JsonValueToAnyStruct(Property, JsonValue);
        if ((InJsonValue->Type == EJson::Array && InJsonValue->AsArray().Num() != 0) || (InJsonValue->Type == EJson::Object && InJsonValue->AsObject()->Values.Num() != 0))
        {
            Success = UAdvanceGameToolLibrary::JsonValueToAnyStruct(InJsonValue, Property, ValuePtr);
        }
    }
    return Success;
}

TSharedRef<FJsonValue> UAdvanceGameToolLibrary::JsonValueToAnyStruct(FProperty* Property, TSharedPtr<FJsonValue> Value)
{
    if (!Value.IsValid() || Value->IsNull() || Property == NULL)
    {
        return MakeShareable(new FJsonValueNull());
    }
    // array
    else if (FArrayProperty* arrayProperty = CastField<FArrayProperty>(Property))
    {
        TArray<TSharedPtr<FJsonValue>> OutArray;
        if (Value->Type == EJson::Array)
        {
            auto InArray = Value->AsArray();
            for (int32 ArrayIndex = 0; ArrayIndex < InArray.Num(); ArrayIndex++)
            {
                OutArray.Add(UAdvanceGameToolLibrary::JsonValueToAnyStruct(arrayProperty->Inner, InArray[ArrayIndex]));
            }
        }
        else
        {
            WarningLog(FString::Printf(TEXT("JsonValueToAnyStruct: error while converting JsonValue to array, type %i is invalid with array property, returning empty array"), Value->Type));
        }
        return MakeShareable(new FJsonValueArray(OutArray));
    }
    // set
    else if (FSetProperty* setProperty = CastField<FSetProperty>(Property))
    {
        TArray<TSharedPtr<FJsonValue>> OutSet;
        if (Value->Type == EJson::Array)
        {
            auto InSet = Value->AsArray();
            for (int32 ArrayIndex = 0; ArrayIndex < InSet.Num(); ArrayIndex++)
            {
                OutSet.Add(UAdvanceGameToolLibrary::JsonValueToAnyStruct(setProperty->ElementProp, InSet[ArrayIndex]));
            }
        }
        else
        {
            WarningLog(FString::Printf(TEXT("JsonValueToAnyStruct: error while converting JsonValue to set, type %i is invalid with set property, returning empty set"), Value->Type));
        }
        return MakeShareable(new FJsonValueArray(OutSet));
    }
    // map
    else if (FMapProperty* mapProperty = CastField<FMapProperty>(Property))
    {
        TSharedRef<FJsonObject> OutMap = MakeShared<FJsonObject>();
        if (Value->Type == EJson::Object)
        {
            auto InMap = Value->AsObject();
            if (InMap)
            {
                for (auto Val : InMap->Values)
                {
                    OutMap->SetField(Val.Key, UAdvanceGameToolLibrary::JsonValueToAnyStruct(mapProperty->ValueProp, Val.Value));
                }
            }
        }
        else
        {
            WarningLog(FString::Printf(TEXT("JsonValueToAnyStruct: error while converting JsonValue to map, type %i is invalid with map property, returning empty object"), Value->Type));
        }
        return MakeShareable(new FJsonValueObject(OutMap));
    }
    // struct
    else if (FStructProperty* structProperty = CastField<FStructProperty>(Property))
    {
        TSharedRef<FJsonObject> OutObject = MakeShared<FJsonObject>();
        if (Value->Type == EJson::Object)
        {
            auto InObject = Value->AsObject();
            if (InObject)
            {
                for (TFieldIterator<FProperty> It(structProperty->Struct); It; ++It)
                {
                    FProperty* Prop = *It;
                    if (InObject->HasField(Prop->GetAuthoredName()))
                    {
                        OutObject->SetField(Prop->GetName(), UAdvanceGameToolLibrary::JsonValueToAnyStruct(Prop, *InObject->Values.Find(Prop->GetAuthoredName())));
                    }
                    else if (InObject->HasField(Prop->GetName()))
                    {
                        OutObject->SetField(Prop->GetName(), UAdvanceGameToolLibrary::JsonValueToAnyStruct(Prop, *InObject->Values.Find(Prop->GetName())));
                    }
                }
            }
        }
        else
        {
            WarningLog(FString::Printf(TEXT("JsonValueToAnyStruct: error while converting JsonValue to struct, type %i is invalid with struct property, returning empty object"), Value->Type));
        }
        return MakeShareable(new FJsonValueObject(OutObject));
    }
    // object
    else if (FObjectProperty* objectProperty = CastField<FObjectProperty>(Property))
    {
        if (objectProperty->PropertyClass->IsNative())
        {
            return Value.ToSharedRef();
        }
        if (Value->Type == EJson::Object)
        {
            auto InObject = Value->AsObject();
            if (InObject.IsValid())
            {
                TSharedRef<FJsonObject> OutObject = MakeShared<FJsonObject>();
                for (TFieldIterator<FProperty> It(objectProperty->PropertyClass); It; ++It)
                {
                    FProperty* Prop = *It;
                    if (InObject->HasField(Prop->GetAuthoredName()))
                    {
                        OutObject->SetField(Prop->GetName(), UAdvanceGameToolLibrary::JsonValueToAnyStruct(Prop, InObject->TryGetField(Prop->GetAuthoredName())));
                    }
                    else if (InObject->HasField(Prop->GetName()))
                    {
                        OutObject->SetField(Prop->GetName(), UAdvanceGameToolLibrary::JsonValueToAnyStruct(Prop, InObject->TryGetField(Prop->GetName())));
                    }
                }
                return MakeShareable(new FJsonValueObject(OutObject));
            }
        }
        else
        {
            WarningLog(FString::Printf(TEXT("JsonValueToAnyStruct: error while converting JsonValue to object, type %i is invalid with object property, returning empty object"), Value->Type));
        }
        return MakeShareable(new FJsonValueNull());
    }
    return Value.ToSharedRef();
}

bool UAdvanceGameToolLibrary::JsonValueToAnyStruct(TSharedPtr<FJsonValue> JsonValue, FProperty* Property, void* ValuePtr)
{
    if (!JsonValue.IsValid() || Property == NULL)
    {
        return false;
    }
    // array
    else if (FArrayProperty* arrayProperty = CastField<FArrayProperty>(Property))
    {
        if (JsonValue->Type == EJson::Array)
        {
            auto JsonArray = JsonValue->AsArray();
            auto Helper = FScriptArrayHelper::CreateHelperFormInnerProperty(arrayProperty->Inner, ValuePtr);
            Helper.Resize(JsonArray.Num());
            for (int32 i = 0; i < JsonArray.Num(); i++)
            {
                if (!UAdvanceGameToolLibrary::JsonValueToAnyStruct(JsonArray[i], arrayProperty->Inner, Helper.GetRawPtr(i)))
                {
                    return false;
                }
            }
        }
    }
    // set
    else if (FSetProperty* setProperty = CastField<FSetProperty>(Property))
    {
        if (JsonValue->Type == EJson::Array)
        {
            auto JsonArray = JsonValue->AsArray();
            auto Helper = FScriptSetHelper::CreateHelperFormElementProperty(setProperty->ElementProp, ValuePtr);
            for (int32 i = 0; i < JsonArray.Num(); i++)
            {
                int32 Idx = Helper.AddDefaultValue_Invalid_NeedsRehash();
                if (!UAdvanceGameToolLibrary::JsonValueToAnyStruct(JsonArray[i], setProperty->ElementProp, Helper.GetElementPtr(Idx)))
                {
                    return false;
                }
            }
            Helper.Rehash();
        }
    }
    // map
    else if (FMapProperty* mapProperty = CastField<FMapProperty>(Property))
    {
        if (JsonValue->Type == EJson::Object)
        {
            auto JsonObject = JsonValue->AsObject();
            auto Helper = FScriptMapHelper::CreateHelperFormInnerProperties(mapProperty->KeyProp, mapProperty->ValueProp, ValuePtr);
            Helper.EmptyValues(JsonObject->Values.Num());

            for (const auto& Entry : JsonObject->Values)
            {
                int32 Idx = Helper.AddDefaultValue_Invalid_NeedsRehash();
                TSharedPtr<FJsonValueString> StrKeyVal = MakeShared<FJsonValueString>(Entry.Key);
                if (!UAdvanceGameToolLibrary::JsonValueToAnyStruct(StrKeyVal, mapProperty->KeyProp, Helper.GetKeyPtr(Idx)) ||
                    !UAdvanceGameToolLibrary::JsonValueToAnyStruct(Entry.Value, mapProperty->ValueProp, Helper.GetValuePtr(Idx)))
                {
                    return false;
                }
            }

            Helper.Rehash();
        }
    }
    // struct
    else if (FStructProperty* structProperty = CastField<FStructProperty>(Property))
    {
        if (JsonValue->Type == EJson::String)
        {
            return FJsonObjectConverter::JsonValueToUProperty(JsonValue, Property, ValuePtr);
        }
        if (JsonValue->Type == EJson::Object)
        {
            auto JsonObject = JsonValue->AsObject();
            for (TFieldIterator<FProperty> It(structProperty->Struct); It; ++It)
            {
                FProperty* Prop = *It;
                if (JsonObject->HasField(Prop->GetName()))
                {
                    void* Value = Prop->ContainerPtrToValuePtr<uint8>(ValuePtr);
                    if (!UAdvanceGameToolLibrary::JsonValueToAnyStruct(JsonObject->TryGetField(Prop->GetName()), Prop, Value))
                    {
                        return false;
                    }
                }
            }
        }
    }
    // object
    else if (FObjectProperty* objectProperty = CastField<FObjectProperty>(Property))
    {
        if (objectProperty->PropertyClass->IsNative())
        {
            return FJsonObjectConverter::JsonValueToUProperty(JsonValue, Property, ValuePtr, 0, 0);
        }
        if (JsonValue->Type == EJson::Object && ValuePtr)
        {
            UObject* PropValue = objectProperty->GetObjectPropertyValue(ValuePtr);
            if (!PropValue)
            {
                PropValue = StaticAllocateObject(objectProperty->PropertyClass, GetTransientPackage(), NAME_None, EObjectFlags::RF_NoFlags, EInternalObjectFlags::None, false);
                (*objectProperty->PropertyClass->ClassConstructor)(FObjectInitializer(PropValue, objectProperty->PropertyClass->ClassDefaultObject, EObjectInitializerOptions::None));
                objectProperty->SetObjectPropertyValue(ValuePtr, PropValue);
            }
            auto JsonObject = JsonValue->AsObject();
            if (!JsonObject->HasTypedField<EJson::String>("_ClassName"))
            {
                JsonObject->SetStringField("_ClassName", PropValue->GetClass()->GetFName().ToString());
            }
            return FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), objectProperty->PropertyClass, PropValue, 0, 0);
        }
    }
    // scalar
    else
    {
        return FJsonObjectConverter::JsonValueToUProperty(JsonValue, Property, ValuePtr, 0, 0);
    }
    return true;
}

#pragma endregion

#pragma endregion
