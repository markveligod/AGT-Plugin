/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AdvanceGameTools/AGTDataTypes.h"
#include "Misc/OutputDeviceNull.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "XmlNode.h"
#include "AdvanceGameToolLibrary.generated.h"

/** Preprocesses for timers **/
#define QUICK_TIMER(World, InObj, Func, Timer, bLoop) UAdvanceGameToolLibrary::QuickTimer(World, InObj, Func, Timer, bLoop)
#define QUICK_TIMER_NEXT_TICK(World, InObj, Func) UAdvanceGameToolLibrary::QuickTimerNextTick(World, InObj, Func)
#define QUICK_TIMER_WITH_DELEGATE(World, TimerDelegate, Timer, bLoop) UAdvanceGameToolLibrary::QuickTimerByTimerDelegate(World, TimerDelegate, Timer, bLoop)
#define QUICK_TIMER_NEXT_TICK_WITH_DELEGATE(World, TimerDelegate) UAdvanceGameToolLibrary::QuickTimerNextTickByTimerDelegate(World, TimerDelegate)
#define CLEAR_TIMER(World, TimerHandle) UAdvanceGameToolLibrary::ClearTimerHandle(World, TimerHandle)
#define CHECK_VALID_TIMER(World, TimerHandle) UAdvanceGameToolLibrary::CheckValidTimerHandle(World, TimerHandle)

class UCanvasPanel;
class UAGTFileHandle;

/**
 * @class ADVANCE GAME TOOL LIBRARY
 */
UCLASS()
class ADVANCEGAMETOOLS_API UAdvanceGameToolLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

#pragma region ActionLogging

public:
    UFUNCTION(BlueprintCallable, Category = "ActionLogging")
    static void ErrorLog(const FString& Message);
    UFUNCTION(BlueprintCallable, Category = "ActionLogging")
    static void WarningLog(const FString& Message);
    UFUNCTION(BlueprintCallable, Category = "ActionLogging")
    static void InfoLog(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "ActionLogging")
    static void ActorErrorLog(AActor* Actor, const FString& Message);
    UFUNCTION(BlueprintCallable, Category = "ActionLogging")
    static void ActorWarningLog(AActor* Actor, const FString& Message);
    UFUNCTION(BlueprintCallable, Category = "ActionLogging")
    static void ActorInfoLog(AActor* Actor, const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "ActionLogging")
    static void DebugLogError(const FString& Message);
    UFUNCTION(BlueprintCallable, Category = "ActionLogging")
    static void DebugLogWarning(const FString& Message);
    UFUNCTION(BlueprintCallable, Category = "ActionLogging")
    static void DebugLogMessage(const FString& Message);

#pragma endregion

#pragma region ActionFiles

public:
    UFUNCTION(BlueprintCallable, Category = "ActionFiles")
    static bool FileSaveString(FString Text, FString FileName);

    UFUNCTION(BlueprintCallable, Category = "ActionFiles")
    static bool FileLoadString(FString FileName, FString& Text);

    /**
     * Open a file handle for reading and writing to a file. Use the file handle object to seek, read, write, etc.
     */
    UFUNCTION(BlueprintCallable, Category = "ActionFiles")
    static UAGTFileHandle* OpenFileHandle(UObject* outer, const FString filePath, const bool forRead, const bool forWrite, bool& success);

    /**
     * Write an array of bytes to a file
     * @param filePath The full path to the file to write to
     * @param allowOverwrite If true, if the file exists it will be overwritten, or the function will return false
     * @param bytesOut The bytes to write out to the file
     * @return True if success
     */
    UFUNCTION(BlueprintCallable, Category = "ActionFiles")
    static bool WriteBytesToFile(const FString filePath, const bool allowOverwrite, const TArray<uint8>& bytesOut);

    /**
     * Read all bytes from a file and put them into bytesIn
     * @param filePath The full path to the file to write to
     * @param bytesIn The bytes to read in from the file
     * @return True if success
     */
    UFUNCTION(BlueprintCallable, Category = "ActionFiles")
    static bool ReadAllBytesFromFile(const FString filePath, TArray<uint8>& bytesIn);

    /**
     * Read bytes from a file and put them into bytesIn
     * The default offset and numBytes will read the entire file. Change the parameters accordingly.
     * @param filePath The full path to the file to write to
     * @param bytesIn The bytes to read in from the file
     * @param offset The offset in bytes into the file
     * @param numBytes The number of bytes at the offset to read
     * @return True if success
     */
    UFUNCTION(BlueprintCallable, Category = "ActionFiles")
    static bool ReadBytesFromFile(const FString filePath, TArray<uint8>& bytesIn, const int64 offset = 0, const int64 numBytes = 99999999999);

#pragma region Paths

public:
    UFUNCTION(BlueprintPure, meta = (DisplayName = "GetEngineDirectories", CompactNodeTitle = "EngineDirs", Keywords = "File plugin path engine directory", ToolTip = "Gets the engine directories"),
        Category = "ActionFiles|Path")
    static FEnginePath GetEngineDirectories();

    UFUNCTION(BlueprintPure,
        meta = (DisplayName = "GetProjectDirectories", CompactNodeTitle = "ProjectDirs", Keywords = "File plugin path project directory", ToolTip = "Gets the project directories"),
        Category = "ActionFiles|Path")
    static FProjectPath GetProjectDirectories();

#pragma endregion

#pragma region TextFile

public:
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReadTextFile", CompactNodeTitle = "ReadText", Keywords = "File plugin read text", ToolTip = "Read a standard text file"),
        Category = "ActionFiles|Text")
    static bool ReadText(FString Path, FString& Output);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "WriteTextFile", CompactNodeTitle = "WriteText", Keywords = "File plugin write text", ToolTip = "Save a standard text file"),
        Category = "ActionFiles|Text")
    static bool SaveText(FString Path, FString Text, FString& Error, bool Append = false, bool Force = false);
    UFUNCTION(BlueprintCallable,
        meta = (DisplayName = "ReadLineFile", CompactNodeTitle = "ReadLine", Keywords = "File plugin read text lines pattern", ToolTip = "Read the lines of a standard text file"),
        Category = "ActionFiles|Text")
    static bool ReadLine(FString Path, FString Pattern, TArray<FString>& Lines);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "WriteLineFile", CompactNodeTitle = "WriteLine", Keywords = "File plugin write text lines", ToolTip = "Save lines in a standard text file"),
        Category = "ActionFiles|Text")
    static bool SaveLine(FString Path, const TArray<FString>& Text, FString& Error, bool Append = false, bool Force = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReadByteFile", CompactNodeTitle = "ReadByte", Keywords = "File plugin read byte", ToolTip = "Read byte file"), Category = "ActionFiles|Byte")
    static bool ReadByte(FString Path, TArray<uint8>& Bytes);
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "WriteByteFile", CompactNodeTitle = "WriteByte", Keywords = "File plugin write byte", ToolTip = "Save byte to file"), Category = "ActionFiles|Byte")
    static bool SaveByte(FString Path, const TArray<uint8>& Bytes, FString& Error, bool Append = false, bool Force = false);

#pragma endregion

#pragma region Base64

public:
    UFUNCTION(BlueprintPure, meta = (DisplayName = "StrToBase64", CompactNodeTitle = "ToBase64", Keywords = "File plugin string convert base64 encode", ToolTip = "Encodes a string to base64"),
        Category = "ActionFiles|Text")
    static FString StringToBase64(const FString Source);
    UFUNCTION(BlueprintPure, meta = (DisplayName = "StrFromBase64", CompactNodeTitle = "FromBase64", Keywords = "File plugin string convert decode base64", ToolTip = "Decodes a string from base64"),
        Category = "ActionFiles|Text")
    static bool StringFromBase64(FString Base64Str, FString& Result);
    UFUNCTION(BlueprintPure, meta = (DisplayName = "BytesToBase64", CompactNodeTitle = "ToBase64", Keywords = "File plugin bytes convert base64 encode", ToolTip = "Encodes a byte array to base64"),
        Category = "ActionFiles|Byte")
    static FString BytesToBase64(const TArray<uint8> Bytes);
    UFUNCTION(BlueprintPure,
        meta = (DisplayName = "BytesFromBase64", CompactNodeTitle = "FromBase64", Keywords = "File plugin bytes convert base64 decode", ToolTip = "Decodes a byte array from base64"),
        Category = "ActionFiles|Byte")
    static bool BytesFromBase64(const FString Source, TArray<uint8>& Out);

#pragma endregion

#pragma region CSVFile

public:
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "WriteCSVFile", CompactNodeTitle = "WriteCSV", Keywords = "File plugin write csv", ToolTip = "Save a csv file"), Category = "ActionFiles|CSV")
    static bool SaveCSV(FString Path, TArray<FString> Headers, TArray<FString> Data, int32& Total, bool Force = false);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReadCSVFile", CompactNodeTitle = "ReadCSV", Keywords = "File plugin read csv", ToolTip = "Read a csv file"), Category = "ActionFiles|CSV")
    static bool ReadCSV(FString Path, TArray<FString>& Headers, TArray<FString>& Data, int32& Total, bool HeaderFirst = true);

#pragma endregion

#pragma region CSVConvert

public:
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "StringToCSV", CompactNodeTitle = "StrToCSV", Keywords = "File plugin string csv", ToolTip = "convert a string to csv"), Category = "ActionFiles|CSV")
    static bool StringToCSV(FString Content, TArray<FString>& Headers, TArray<FString>& Data, int32& Total, bool HeaderFirst = true);

    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "CSVToString", CompactNodeTitle = "CSVToStr", Keywords = "File plugin csv string", ToolTip = "convert a csv to string"), Category = "ActionFiles|CSV")
    static bool CSVToString(FString& Result, TArray<FString> Headers, TArray<FString> Data, int32& Total);

#pragma endregion

#pragma region XMLFile

public:
    UFUNCTION(BlueprintCallable, Category = "ActionFiles|XMLFile", CustomThunk, meta = (CustomStructureParam = "InStruct"))
    static void StructToXML(FString& XML, bool& Success, const UStruct* InStruct);
    DECLARE_FUNCTION(execStructToXML)
    {
        P_GET_PROPERTY_REF(FStrProperty, XML);
        P_GET_UBOOL_REF(Success);

        Stack.Step(Stack.Object, NULL);

        FProperty* Property = Stack.MostRecentProperty;
        void* Ptr = Stack.MostRecentPropertyAddress;

        P_FINISH;

        Success = UAdvanceGameToolLibrary::AnyStructToXmlString(Property, Ptr, XML);
    }

    UFUNCTION(BlueprintCallable, Category = "ActionFiles|XMLFile", CustomThunk, meta = (CustomStructureParam = "OutStruct"))
    static void XMLToStruct(const FString& Xml, bool& Success, UStruct*& OutStruct);
    DECLARE_FUNCTION(execXMLToStruct)
    {
        // P_GET_SOFTCLASS_REF(int32, Total);
        P_GET_PROPERTY(FStrProperty, Xml);
        P_GET_UBOOL_REF(Success);

        Stack.Step(Stack.Object, NULL);

        FProperty* Property = Stack.MostRecentProperty;
        void* Ptr = Stack.MostRecentPropertyAddress;

        P_FINISH;

        Success = UAdvanceGameToolLibrary::XmlStringToAnyStruct(Property, Ptr, Xml);
    }

    // xml v2
    static bool AnyStructToXmlString(FProperty* Property, void* ValuePtr, FString& Xml);
    static FString AnyStructToXmlValue(FProperty* Property, TSharedPtr<FJsonValue> Value, int32 Depth);
    static FString& CreateTagNode(FString Tag, FString& Content, int32 Depth, bool WithCR);
    static bool XmlStringToAnyStruct(FProperty* Property, void* ValuePtr, const FString& Xml);
    static TSharedRef<FJsonValue> XmlNodeToAnyStruct(FProperty* Property, FXmlNode* Node);
    // xml escape
    static FString XmlEscapeChars(FString Source);
    static FString XmlConvertChars(FString Source);

#pragma endregion

#pragma region JSONFile

public:
    UFUNCTION(BlueprintCallable, Category = "ActionFiles|JSONFile", CustomThunk, meta = (CustomStructureParam = "InStruct"))
    static void StructToJson(FString& Json, bool& Success, const UStruct* InStruct);
    DECLARE_FUNCTION(execStructToJson)
    {
        P_GET_PROPERTY_REF(FStrProperty, Json);
        P_GET_UBOOL_REF(Success);

        Stack.Step(Stack.Object, NULL);

        FProperty* Prop = Stack.MostRecentProperty;
        void* Ptr = Stack.MostRecentPropertyAddress;

        P_FINISH;

        Success = UAdvanceGameToolLibrary::AnyStructToJsonString(Prop, Ptr, Json);
    }

    UFUNCTION(BlueprintCallable, Category = "ActionFiles|JSONFile", CustomThunk, meta = (CustomStructureParam = "OutStruct"))
    static void JsonToStruct(const FString& Json, bool& Success, UStruct*& OutStruct);
    DECLARE_FUNCTION(execJsonToStruct)
    {
        // P_GET_SOFTCLASS_REF(int32, Total);
        P_GET_PROPERTY(FStrProperty, Json);
        P_GET_UBOOL_REF(Success);

        Stack.Step(Stack.Object, NULL);

        FProperty* Prop = Stack.MostRecentProperty;
        void* Ptr = Stack.MostRecentPropertyAddress;

        P_FINISH;

        Success = UAdvanceGameToolLibrary::JsonStringToAnyStruct(Prop, Ptr, Json);
    }

#pragma endregion

#pragma region FileSystem

public:
    /* File system */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "IsFile", CompactNodeTitle = "IsFile", Keywords = "File plugin check file exist", ToolTip = "Check whether a file exists"),
        Category = "ActionFiles|FileSystem")
    static bool IsFile(FString Path);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "IsDirectory", CompactNodeTitle = "IsDir", Keywords = "File plugin check directory exist", ToolTip = "Check whether a directory exists"),
        Category = "ActionFiles|FileSystem")
    static bool IsDirectory(FString Path);
    UFUNCTION(BlueprintPure,
        meta = (DisplayName = "IsValidFilename", CompactNodeTitle = "IsValidName", Keywords = "File plugin check path valid", ToolTip = "Check whether a filename is valid and can be used"),
        Category = "ActionFiles|FileSystem")
    static bool IsValidFilename(FString Filename);
    UFUNCTION(BlueprintPure,
        meta = (DisplayName = "ValidateFilename", CompactNodeTitle = "ValidateName", Keywords = "File plugin validate path", ToolTip = "Validate a filename to be used on this file system"),
        Category = "ActionFiles|FileSystem")
    static bool ValidateFilename(FString Filename, FString& ValidName);
    UFUNCTION(BlueprintCallable,
        meta = (DisplayName = "SetReadOnlyFlag", CompactNodeTitle = "SetReadOnly", Keywords = "File plugin read only path", ToolTip = "Updates the read only property on file"),
        Category = "ActionFiles|FileSystem")
    static bool SetReadOnlyFlag(FString FilePath, bool Flag);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetReadOnlyFlag", CompactNodeTitle = "IsReadOnly", Keywords = "File plugin read only path", ToolTip = "Gets the read only property on file"),
        Category = "ActionFiles|FileSystem")
    static bool GetReadOnlyFlag(FString FilePath);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetFileSize", CompactNodeTitle = "GetSize", Keywords = "File plugin size directory", ToolTip = "Gets the size of a file"),
        Category = "ActionFiles|FileSystem")
    static int64 GetFileSize(FString FilePath);
    UFUNCTION(BlueprintCallable,
        meta = (DisplayName = "ListDirectory", CompactNodeTitle = "LsDir", Keywords = "File plugin list directory pattern regex recursive", ToolTip = "List nodes from directory"),
        Category = "ActionFiles|FileSystem")
    static bool ListDirectory(FString Path, FString Pattern, TArray<FString>& Nodes, bool ShowFile = true, bool ShowDirectory = true, bool Recursive = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "MakeDirectory", CompactNodeTitle = "MkDir", Keywords = "File plugin make directory recursive", ToolTip = "Create a new directory"),
        Category = "ActionFiles|FileSystem")
    static bool MakeDirectory(FString Path, bool Recursive = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "RemoveDirectory", CompactNodeTitle = "RmDir", Keywords = "File plugin remove directory recursive", ToolTip = "Removes a directory"),
        Category = "ActionFiles|FileSystem")
    static bool RemoveDirectory(FString Path, bool Recursive = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "CopyDirectory", CompactNodeTitle = "CpDir", Keywords = "File plugin copy directory recursive", ToolTip = "Copies a directory"),
        Category = "ActionFiles|FileSystem")
    static bool CopyDirectory(FString Source, FString Dest);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "MoveDirectory", CompactNodeTitle = "MvDir", Keywords = "File plugin move directory recursive", ToolTip = "Moves a directory"),
        Category = "ActionFiles|FileSystem")
    static bool MoveDirectory(FString Source, FString Dest);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "NodeStats", CompactNodeTitle = "NodeStats", Keywords = "File plugin stats directory node", ToolTip = "Gets the stats of a node"),
        Category = "ActionFiles|FileSystem")
    static bool NodeStats(FString Path, FCustomNodeStat& Stats);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "RemoveFile", CompactNodeTitle = "RmFile", Keywords = "File plugin remove file recursive", ToolTip = "Removes a file"),
        Category = "ActionFiles|FileSystem")
    static bool RemoveFile(FString Path);
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "CopyFile", CompactNodeTitle = "CpFile", Keywords = "File plugin copy file recursive", ToolTip = "Copies a file"), Category = "ActionFiles|FileSystem")
    static bool CopyFile(FString Source, FString Dest, bool Force = false);
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "MoveFile", CompactNodeTitle = "MvFile", Keywords = "File plugin move file recursive", ToolTip = "Moves a file"), Category = "ActionFiles|FileSystem")
    static bool MoveFile(FString Source, FString Dest, bool Force = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "RenameFile", CompactNodeTitle = "RenameFile", Keywords = "File plugin rename file recursive", ToolTip = "Renames a file"),
        Category = "ActionFiles|FileSystem")
    static bool RenameFile(FString Path, FString NewName);
    UFUNCTION(BlueprintPure, meta = (DisplayName = "PathParts", Keywords = "File plugin path parts", ToolTip = "Gets the parts of a path"), Category = "ActionFiles|FileSystem")
    static void GetPathParts(FString Path, FString& PathPart, FString& BasePart, FString& ExtensionPart, FString& FileName);

#pragma endregion

#pragma region Screenshot

public:
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "TakeScreenshot", CompactNodeTitle = "Screenshot", Keywords = "File plugin screenshot save image", ToolTip = "Take a screenshot and save"),
        Category = "ActionFiles|Screenshot")
    static bool TakeScreenShot(FString Filename, FString& Path, bool PrefixTimestamp = true, bool ShowUI = true);
    UFUNCTION(BlueprintCallable,
        meta = (DisplayName = "LoadScreenshot", CompactNodeTitle = "LoadScreenshot", Keywords = "File plugin texture read screenshot", ToolTip = "load a screenshot from image file"),
        Category = "ActionFiles|Screenshot")
    static UTexture2D* LoadScreenshot(FString Path, bool& Success);

#pragma endregion

#pragma region DataTable

public:
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "DataTableToCSV", Keywords = "File plugin datatable csv convert export", ToolTip = "Converts a datatable to csv string"),
        Category = "ActionFiles|Datatable")
    static bool DatatableToCSV(UDataTable* Table, FString& Output);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "DataTableToJSON", Keywords = "File plugin datatable json convert export", ToolTip = "Converts a datatable to json string"),
        Category = "ActionFiles|Datatable")
    static bool DataTableToJSON(UDataTable* Table, FString& Output);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "CSVToDataTable", Keywords = "File plugin datatable csv convert import", ToolTip = "Converts a csv string to datatable"),
        Category = "ActionFiles|Datatable")
    static UDataTable* CSVToDataTable(FString CSV, UScriptStruct* Struct, bool& Success);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "JSONToDataTable", Keywords = "File plugin datatable json convert import", ToolTip = "Converts a json string to datatable"),
        Category = "ActionFiles|Datatable")
    static UDataTable* JSONToDataTable(FString JSON, UScriptStruct* Struct, bool& Success);

    // datatable csv
    static bool WriteTableToCSV(const UDataTable& InDataTable, FString& Output);
    static bool WriteRowToCSV(const UScriptStruct* InRowStruct, const void* InRowData, FString& ExportedText);
    static bool WriteStructEntryToCSV(const void* InRowData, FProperty* InProperty, const void* InPropertyData, FString& ExportedText);

    // datatable json
    static FString GetKeyFieldName(const UDataTable& InDataTable);
    static bool WriteTableToJSON(const UDataTable& InDataTable, FString& OutExportText);
    static bool WriteTableAsObjectToJSON(const UDataTable& InDataTable, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);
    static bool WriteRowToJSON(const UScriptStruct* InRowStruct, const void* InRowData, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);
    static bool WriteStructToJSON(const UScriptStruct* InStruct, const void* InStructData, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);
    static bool WriteStructEntryToJSON(const void* InRowData, const FProperty* InProperty, const void* InPropertyData, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);
    static bool WriteContainerEntryToJSON(
        const FProperty* InProperty, const void* InPropertyData, const FString* InIdentifier, TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter);
    static void WriteJSONObjectStartWithOptionalIdentifier(TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter, const FString* InIdentifier);
    static void WriteJSONValueWithOptionalIdentifier(TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter, const FString* InIdentifier, const TCHAR* InValue);

#pragma endregion

#pragma region ConfigFileINI

public:
    UFUNCTION(BlueprintCallable, Category = "ActionFiles|Config", CustomThunk, meta = (CustomStructureParam = "OutValue"))
    static void ReadConfig(FString FilePath, FString Section, FString Key, bool& Success, bool SingleLineArrayRead, UStruct*& OutValue);
    DECLARE_FUNCTION(execReadConfig)
    {
        P_GET_PROPERTY(FStrProperty, FilePath);
        P_GET_PROPERTY(FStrProperty, Section);
        P_GET_PROPERTY(FStrProperty, Key);
        P_GET_UBOOL_REF(Success);
        P_GET_UBOOL(SingleLineArrayRead);

        Stack.Step(Stack.Object, NULL);

        FProperty* Property = Stack.MostRecentProperty;
        void* ValuePtr = Stack.MostRecentPropertyAddress;

        P_FINISH;

        Success = UAdvanceGameToolLibrary::ReadConfigFile(FilePath, Section, Key, Property, ValuePtr, SingleLineArrayRead);
    }

    UFUNCTION(BlueprintCallable, Category = "ActionFiles|Config", CustomThunk, meta = (CustomStructureParam = "Value"))
    static void WriteConfig(FString FilePath, FString Section, FString Key, bool& Success, bool SingleLineArrayWrite, const UStruct* Value);
    DECLARE_FUNCTION(execWriteConfig)
    {
        P_GET_PROPERTY(FStrProperty, FilePath);
        P_GET_PROPERTY(FStrProperty, Section);
        P_GET_PROPERTY(FStrProperty, Key);
        P_GET_UBOOL_REF(Success);
        P_GET_UBOOL(SingleLineArrayWrite);

        Stack.Step(Stack.Object, NULL);

        FProperty* Property = Stack.MostRecentProperty;
        void* ValuePtr = Stack.MostRecentPropertyAddress;

        P_FINISH;

        Success = UAdvanceGameToolLibrary::WriteConfigFile(FilePath, Section, Key, Property, ValuePtr, SingleLineArrayWrite);
    }

    UFUNCTION(BlueprintCallable, Category = "ActionFiles|Config")
    static bool RemoveConfig(FString FilePath, FString Section, FString Key);

    // config ini
    static bool WriteConfigFile(FString Filename, FString Section, FString Key, FProperty* Type, void* Value, bool SingleLineArray);
    static bool ReadConfigFile(FString Filename, FString Section, FString Key, FProperty* Type, void* Value, bool SingleLineArray);

#pragma endregion

#pragma region JSONV2

    // json v2
    static bool AnyStructToJsonString(FProperty* Property, void* ValuePtr, FString& Json);
    static TSharedRef<FJsonValue> AnyStructToJsonValue(FProperty* Property, void* ValuePtr);
    static bool JsonStringToAnyStruct(FProperty* Property, void* ValuePtr, const FString& Json);
    static TSharedRef<FJsonValue> JsonValueToAnyStruct(FProperty* Property, TSharedPtr<FJsonValue> Value);
    static bool JsonValueToAnyStruct(TSharedPtr<FJsonValue> JsonValue, FProperty* Property, void* ValuePtr);

#pragma endregion

#pragma endregion

#pragma region ModeState

public:
    UFUNCTION(BlueprintPure, Category = "CheckedState")
    static bool IsEditor();

    UFUNCTION(BlueprintPure, Category = "CheckedState")
    static bool IsBuildDebug();

    UFUNCTION(BlueprintPure, Category = "CheckedState")
    static bool IsBuildDevelopment();

    UFUNCTION(BlueprintPure, Category = "CheckedState")
    static bool IsBuildShipping();

    /** @public Switching platform **/
    UFUNCTION(BlueprintCallable, Category = "CheckedState", Meta = (ExpandEnumAsExecs = "PlatformType"))
    static void SwitchPlatform(EPlatformType& PlatformType);

    /** @public Net mode to string **/
    UFUNCTION(BlueprintPure, Category = "CheckedState")
    static FString GetNetModeToString(AActor* InActor);

#pragma endregion

#pragma region ActionObject

public:
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static UObject* GetDefaultObjectFromClass(UClass* Class) { return Class ? Class->GetDefaultObject() : nullptr; }

    template <typename T>
    static T* ToCastSyncAssetLoadObject(TSoftObjectPtr<T> AssetObject)
    {
        return Cast<T>(SyncAssetLoadObject(AssetObject));
    }

    /** @public Synchronously load asset **/
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static UObject* SyncAssetLoadObject(TSoftObjectPtr<UObject> AssetObject);

    /** @public Synchronously load asset and spawn actor then */
    UFUNCTION(BlueprintCallable, Category = "ActionObject", meta = (WorldContext = "WorldContextObject"))
    static AActor* SyncSpawnActor(UObject* WorldContextObject, TSoftClassPtr<AActor> AssetClass, FAdvanceActorParameters ActorParameters);

    /** @public Asynchronously load asset */
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static void AsyncAssetLoadObject(TSoftObjectPtr<UObject> AssetObject, const FAsyncAssetLoadSignature& Callback, bool bHighPriority);

    /** @public Asynchronously load asset and spawn actor then */
    UFUNCTION(BlueprintCallable, Category = "ActionObject", meta = (WorldContext = "WorldContextObject"))
    static void AsyncSpawnActor(UObject* WorldContextObject, TSoftClassPtr<AActor> AssetClass, FAdvanceActorParameters ActorParameters, const FAsyncSpawnActorSignature& Callback, bool bHighPriority);

    /** @public Unload asset */
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static void UnloadObject(TSoftObjectPtr<UObject> AssetObject);

    /** @public Unload actor */
    UFUNCTION(BlueprintCallable, Category = "ActionObject", meta = (WorldContext = "WorldContextObject"))
    static void UnloadActor(TSoftClassPtr<AActor> AssetClass);

    /** @public Async task for short tasks.
        Be careful! Crash when use more than core threads available. */
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static void DoAsyncTask(FBPAsyncSignature Task, EThreadTaskType ThreadType);

    /** @public Async task for long tasks.
        Be careful! Crash when use more than core threads available. */
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static void DoAsyncLongTask(FBPAsyncSignature Task, const FString& ThreadName, EThreadPriorityType ThreadPriority);

    /** @public Do Num similar tasks parallel. **/
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static void ParallelForTask(FBPParallelForSignature Task, int32 Num, bool ForceSingleThread, bool PumpRenderingThread);

private:
    /** @private Called when asset loading is finished */
    static void OnAssetLoadObjectComplete(TSoftObjectPtr<UObject> AssetObject, FAsyncAssetLoadSignature Callback);

    /** @private  Called when asset loading for actor spawn is finished */
    static void OnSpawnActorComplete(UObject* WorldContextObject, TSoftClassPtr<AActor> AssetClass, FAdvanceActorParameters ActorParameters, FAsyncSpawnActorSignature Callback);

public:
    /** @public Remove all latent actions for an object. Latent actions are delays, async requests, etc. Use this with caution.
     This can be useful for stopping an actor completely (disabled all delays when turning tick off) **/
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "ActionObject")
    static void RemoveLatentActionsForObject(UObject* WorldContextObject, UObject* object);

    /** @public Clear all timers running for an object. Use this with caution. **/
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "ActionObject")
    static void ClearAllTimersForObject(UObject* WorldContextObject, UObject* object);

    /** @public Create a new object of objectClass **/
    UFUNCTION(BlueprintCallable, Category = "ActionObject", Meta = (DeterminesOutputType = "objectClass", DynamicOutputParam = "objectOut"))
    static void CreateObject(TSubclassOf<UObject> objectClass, UObject* outer, UObject*& objectOut);

    /** @public Duplicate a current object **/
    UFUNCTION(BlueprintCallable, Category = "ActionObject", Meta = (DeterminesOutputType = "objectClass", DynamicOutputParam = "objectOut"))
    static void DuplicateObject(TSubclassOf<UObject> objectClass, UObject* object, UObject* outer, UObject*& objectOut);

    /** @public Returns true if the Soft Object Reference is not null AND not pending kill */
    UFUNCTION(BlueprintPure, Category = "ActionObject", meta = (BlueprintThreadSafe))
    static bool IsLiveSoftObjectReference(const TSoftObjectPtr<UObject>& SoftObjectReference);

    /** Returns string representation of reference, in form /package/path.assetname */
    UFUNCTION(BlueprintPure, Category = "ActionObject", meta = (BlueprintThreadSafe))
    static FString SoftObjectToString(const TSoftObjectPtr<UObject>& SoftObjectReference);

    /**
     * @public Test if this does not point to a live UObject, but may in the future
     *
     * @return true if this does not point to a real object, but could possibly
     */
    UFUNCTION(BlueprintPure, Category = "ActionObject", meta = (BlueprintThreadSafe))
    static bool IsSoftObjectPending(const TSoftObjectPtr<UObject>& SoftObjectReference);

    /**
     * @public Test if this can never point to a live UObject
     *
     * @return true if this is explicitly pointing to no object
     */
    UFUNCTION(BlueprintPure, Category = "ActionObject", meta = (BlueprintThreadSafe))
    static bool IsSoftObjectNull(const TSoftObjectPtr<UObject>& SoftObjectReference);

    /**
     * @public Test if this points to a live UObject
     *
     * @return true if Get() would return a valid non-null pointer
     */
    UFUNCTION(BlueprintPure, Category = "ActionObject", meta = (BlueprintThreadSafe))
    static bool IsSoftObjectValid(const TSoftObjectPtr<UObject>& SoftObjectReference);

    /** @public  A call to see if a package is currently loaded and load that. If not loaded, tries to load the package.
     * Package path is in this format: /Game/MyFolder/MyPackage
     * Where /Game/ is the mounting point.
     * Common mount points are:
     * /Game/* : This is your projects primary content folder
     * /Plugin/* : If a plugin has content the mounting point will be the name of the plugin
     * /Engine/* : Content found in the engine
     * **/
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static UPackage* FindOrLoadPackage(const FString& PackageName);

    // Loads an object in a package
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static UObject* LoadObjectFromPackage(UPackage* package, const FString& objectName);

    // Gets the package of an object. Returns None if this object isn't part of a package
    UFUNCTION(BlueprintPure, Category = "ActionObject")
    static UPackage* GetPackageOfObject(UObject* object);

    // With a package, returns all objects within that package
    // NOTE: This loads every asset within the package so it can be returned.
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static void GetObjectsInPackage(UPackage* package, TArray<UObject*>& ObjectsOut);

    /**
     * @public Tries to convert the supplied relative or absolute filename to a long package name/path starting with a root like /game
     * This works on both package names and directories, and it does not validate that it actually exists on disk.
     *
     * @param InFilename Filename to convert.
     * @param OutPackageName The resulting long package name if the conversion was successful.
     * @param OutFailureReason Description of an error if the conversion failed.
     * @return Returns true if the supplied filename properly maps to one of the long package roots.
     */
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static bool TryConvertFilenameToLongPackageName(const FString& InFilename, FString& OutPackageName, FString& OutFailureReason);

    /** @public A call to see if a package asset is currently loaded and load that. If not loaded, tries to load the package asset.
     * Package path is in this format: /Game/MyFolder/MyPackage.MyAsset
     * Where /Game/ is the mounting point.
     * Common mount points are:
     * /Game/* : This is your projects primary content folder
     * /Plugin/* : If a plugin has content the mounting point will be the name of the plugin
     * /Engine/* : Content found in the engine
     **/
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static UObject* LoadObjectWithFullPath(const FString& fullObjectPath);

    UFUNCTION(BlueprintCallable, meta = (Latent = "", LatentInfo = "LatentInfo", WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "ActionObject")
    static void LoadAssetPriority(UObject* WorldContextObject, const TSoftObjectPtr<UObject> Asset, const int32 Priority, FOnAssetLoaded OnLoaded, FLatentActionInfo LatentInfo);

    UFUNCTION(BlueprintCallable, meta = (Latent = "", LatentInfo = "LatentInfo", WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "ActionObject")
    static void LoadPackagePriority(UObject* WorldContextObject, const FString& PackagePath, const int32 Priority, const bool BlockOnLoad, FOnPackageLoaded OnLoaded, FLatentActionInfo LatentInfo);

    /**
     * @public This will insert a mount point at the head of the search chain (so it can overlap an existing mount point and win).
     * This function can be used to mount a path to a simple mounting point name. ex:
     * ../../../MyProject/Plugins/MyDLC/Content to /MyDLC/
     *
     * @param RootPath Logical Root Path. ex: /MyDLC/
     * @param ContentPath Content Path on disk. ex: ../../../MyProject/Plugins/MyDLC/Content
     */
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static void RegisterMountPoint(const FString& RootPath, const FString& ContentPath);

    /**
     * @public This will remove a previously inserted mount point.
     *
     * @param RootPath Logical Root Path.
     * @param ContentPath Content Path on disk.
     */
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static void UnRegisterMountPoint(const FString& RootPath, const FString& ContentPath);

    /**
     * @public Returns whether the specific logical root path is a valid mount point.
     */
    UFUNCTION(BlueprintPure, Category = "ActionObject")
    static bool MountPointExists(const FString& RootPath);

    /**
     * @public Get the mount point for a given package path
     *
     * @param InPackagePath The package path to get the mount point for
     * @param InWithoutSlashes Optional parameters that keeps the slashes around the mount point if false
     * @return FName corresponding to the mount point, or Empty if invalid
     */
    UFUNCTION(BlueprintPure, Category = "ActionObject")
    static FName GetPackageMountPoint(const FString& InPackagePath, bool InWithoutSlashes = true);

    // Return the parent class of a class
    UFUNCTION(BlueprintPure, Category = "ActionObject")
    static UClass* GetParentClass(UClass* Class);

    // Return the class hierarchy in an array ordered from children to root parent
    UFUNCTION(BlueprintPure, Category = "ActionObject")
    static void GetClassHierarchy(UClass* Class, TArray<UClass*>& ClassHierarchy, const bool includeSelf = true);

    // Returns the default object associated with this class.
    UFUNCTION(BlueprintCallable, Category = "ActionObject")
    static UObject* GetClassDefaultObject(TSubclassOf<UObject> TheClass);

    /** @public Sets an objects property by name using reflection to a Value.
     * @WARNING: You can do naughty things with this like setting native variables not exposed to blueprints. Beware!
     * @NOTE: This only supports setting these types: Numeric (Int, Float, Byte, etc), Bool, LinearColor, Color, Vector, Rotator, String.
     *       With struct types like Vector, the Value string must be in a text serializable form ex (X=20, Y=30, Z=40). Use ToString on the types for examples.
     * @NOTE: If the property is numeric, ensure your Value can actually work with that type. If the value can be converted but precision is lost, no warnings will occur.
     **/
    UFUNCTION(BlueprintCallable, Category = "ActionObject", meta = (AdvancedDisplay = "3"))
    static bool SetObjectPropertyValue(UObject* Object, const FName PropertyName, const FString& Value, const bool PrintWarnings = true);

    // This object is a template for another object - treat like a class default object
    UFUNCTION(BlueprintPure, Category = "ActionObject|Flags")
    static bool ObjectHasFlag_ArchetypeObject(UObject* Object);

    // Object is a CDO or Class Default Object. It exists as a template for object that will be created and can be used
    // to get default values.
    UFUNCTION(BlueprintPure, Category = "ActionObject|Flags")
    static bool ObjectHasFlag_ClassDefaultObject(UObject* Object);

    // Object has begun being destroyed
    UFUNCTION(BlueprintPure, Category = "ActionObject|Flags")
    static bool ObjectHasFlag_BeginDestroyed(UObject* Object);

    // Object has finished being destroyed
    UFUNCTION(BlueprintPure, Category = "ActionObject|Flags")
    static bool ObjectHasFlag_FinishDestroyed(UObject* Object);

    // Objects flagged with WasLoaded can be expected to have been loaded from a package and was not dynamically created.
    // NOTE: Objects which were created for a package but not saved yet will not be set as "WasLoaded" until they are saved.
    UFUNCTION(BlueprintPure, Category = "ActionObject|Flags")
    static bool ObjectHasFlag_WasLoaded(UObject* Object);

#pragma endregion

#pragma region NavSystem

public:
    UFUNCTION(BlueprintCallable, Category = "NavSystem", meta = (WorldContext = "WorldContextObject"))
    static void ReloadNavMeshByWorld(UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, Category = "NavSystem")
    static void ReloadNavMesh(UNavigationSystemV1* NavSystem);

    UFUNCTION(BlueprintCallable, Category = "NavSystem")
    static void ReloadNavMeshes(TArray<UNavigationSystemV1*> NavSystemArray);

    UFUNCTION(BlueprintCallable, Category = "NavSystem", meta = (WorldContext = "WorldContextObject"))
    static TArray<FVector> FindPathToLocationByCharacter(UObject* WorldContextObject, FVector Location, ACharacter* Character);

#pragma endregion

#pragma region ActionPlatform

public:
    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetCPUBrandName();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetCPUVendorName();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetGPUBrandName();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static int32 GetCPUCores();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetAudioDeviceName();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static int32 GetMonitorsCount();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static int32 GetPrimaryMonitor();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetOSVersion();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetTotalPhysicalGB();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetMotherboardName();

    UFUNCTION(BlueprintCallable, Category = "ActionPlatform")
    static bool SetActiveMonitor(int32 MonitorIndex);

    UFUNCTION(BlueprintCallable, Category = "ActionPlatform")
    static void SetGamma(float Gamma);

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static float GetGamma();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetProjectVersion();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetProjectName();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetCompanyName();

    UFUNCTION(BlueprintPure, Category = "ActionPlatform")
    static FString GetCopyrightNotice();

#pragma endregion

#pragma region AdvanceArray

public:
#pragma region Average

    template <typename T>
    static float Average(const TArray<T>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "AverageInteger", Keywords = "Average plugin Array Integer", ToolTip = "Get the average of an array"), Category = "Average")
    static float AverageInteger(const TArray<int32>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "AverageFloat", Keywords = "Average plugin Array Float", ToolTip = "Get the average of an array"), Category = "Average")
    static float AverageFloat(const TArray<float>& Array);

#pragma endregion

#pragma region Minimum

    template <typename T>
    static int32 Minimum(const TArray<T>& Array);
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "MinimumIntegerIndex", Keywords = "Minimum plugin Array Integer Index", ToolTip = "Get the minimum value index of an array"), Category = "Minimum")
    static int32 MinimumIntegerIndex(const TArray<int32>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "MinimumFloatIndex", Keywords = "Minimum plugin Array Float Index", ToolTip = "Get the minimum value index of an array"), Category = "Minimum")
    static int32 MinimumFloatIndex(const TArray<float>& Array);

#pragma endregion

#pragma region Maximum

    template <typename T>
    static int32 Maximum(const TArray<T>& Array);
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "MaximumIntegerIndex", Keywords = "Maximum plugin Array Integer Index", ToolTip = "Get the maximum value index of an array"), Category = "Maximum")
    static int32 MaximumIntegerIndex(const TArray<int32>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "MaximumFloatIndex", Keywords = "Maximum plugin Array Float Index", ToolTip = "Get the maximum value index of an array"), Category = "Maximum")
    static int32 MaximumFloatIndex(const TArray<float>& Array);

#pragma endregion

#pragma region Normalize

    template <typename T>
    static TArray<T> MinMaxNormalization(const TArray<T>& Array, T Min, T Max);
    UFUNCTION(BlueprintCallable,
        meta = (DisplayName = "MinMaxFloatNormalization", Keywords = "MinMaxNormalization plugin Array Float", ToolTip = "Normalize the number of an array between two values"),
        Category = "Normalization")
    static TArray<float> MinMaxFloatNormalization(const TArray<float>& Array, float Min = 0, float Max = 1);
    UFUNCTION(BlueprintCallable,
        meta = (DisplayName = "MinMaxIntegerNormalization", Keywords = "MinMaxNormalization plugin Array Integer", ToolTip = "Normalize the number of an array between two values"),
        Category = "Normalization")
    static TArray<int32> MinMaxIntegerNormalization(const TArray<int32>& Array, int32 Min = 0, int32 Max = 100);

#pragma endregion

#pragma region Reverse

    template <typename T>
    static TArray<T> Reverse(const TArray<T>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReverseInteger", Keywords = "Reverse plugin Array Integer", ToolTip = "Reverse an array by copy"), Category = "Reverse")
    static TArray<int32> ReverseInteger(const TArray<int32>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReverseFloat", Keywords = "Reverse plugin Array Float", ToolTip = "Reverse an array by copy"), Category = "Reverse")
    static TArray<float> ReverseFloat(const TArray<float>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReverseString", Keywords = "Reverse plugin Array String", ToolTip = "Reverse an array by copy"), Category = "Reverse")
    static TArray<FString> ReverseString(const TArray<FString>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReverseName", Keywords = "Reverse plugin Array Name", ToolTip = "Reverse an array by copy"), Category = "Reverse")
    static TArray<FName> ReverseName(const TArray<FName>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReverseVector", Keywords = "Reverse plugin Array Vector", ToolTip = "Reverse an array by copy"), Category = "Reverse")
    static TArray<FVector> ReverseVector(const TArray<FVector>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReverseActor", Keywords = "Reverse plugin Array Actor", ToolTip = "Reverse an array by copy"), Category = "Reverse")
    static TArray<AActor*> ReverseActor(const TArray<AActor*>& Array);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ReverseObject", Keywords = "Reverse plugin Array Object", ToolTip = "Reverse an array by copy"), Category = "Reverse")
    static TArray<UObject*> ReverseObject(const TArray<UObject*>& Array);

#pragma endregion

#pragma region Convert

    UFUNCTION(BlueprintPure, meta = (DisplayName = "SplitString", Keywords = "Convert plugin Array String Split Explode Separator", ToolTip = "Convert a string to array"), Category = "Convert")
    static TArray<FString> SplitString(FString String, const FString& Separator, ESearchCase::Type SearchCase, bool RemoveEmptyString = true);
    UFUNCTION(
        BlueprintPure, meta = (DisplayName = "SplitStringBySizeToArray", Keywords = "Convert plugin Array String Split Explode size", ToolTip = "Convert a string to array"), Category = "Convert")
    static TArray<FString> SplitStringBySizeToArray(FString String, int32 Size);
    UFUNCTION(
        BlueprintPure, meta = (DisplayName = "SplitStringBySizeToOneLine", Keywords = "Convert plugin Array String Split Explode size", ToolTip = "Convert a string to array"), Category = "Convert")
    static FString SplitStringBySizeToOneLine(FString String, int32 Size);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ToIntegerSet", Keywords = "Convert plugin Array Integer Set", ToolTip = "Convert an array to set"), Category = "Convert")
    static inline TSet<int32> ToIntegerSet(const TArray<int32>& Array) { return TSet<int32>(Array); };
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ToFloatSet", Keywords = "Convert plugin Array Float Set", ToolTip = "Convert an array to set"), Category = "Convert")
    static inline TSet<float> ToFloatSet(const TArray<float>& Array) { return TSet<float>(Array); };
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ToNameSet", Keywords = "Convert plugin Array Name Set", ToolTip = "Convert an array to set"), Category = "Convert")
    static inline TSet<FName> ToNameSet(const TArray<FName>& Array) { return TSet<FName>(Array); };
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ToStringSet", Keywords = "Convert plugin Array String Set", ToolTip = "Convert an array to set"), Category = "Convert")
    static inline TSet<FString> ToStringSet(const TArray<FString>& Array) { return TSet<FString>(Array); };
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ToVectorSet", Keywords = "Convert plugin Array Vector Set", ToolTip = "Convert an array to set"), Category = "Convert")
    static inline TSet<FVector> ToVectorSet(const TArray<FVector>& Array) { return TSet<FVector>(Array); };
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ToActorSet", Keywords = "Convert plugin Array Actor Set", ToolTip = "Convert an array to set"), Category = "Convert")
    static inline TSet<AActor*> ToActorSet(const TArray<AActor*>& Array) { return TSet<AActor*>(Array); };
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ToObjectSet", Keywords = "Convert plugin Array Object Set", ToolTip = "Convert an array to set"), Category = "Convert")
    static inline TSet<UObject*> ToObjectSet(const TArray<UObject*>& Array) { return TSet<UObject*>(Array); };

#pragma endregion

#pragma region Clamp

    template <typename T>
    static TArray<T> Clamp(TArray<T>& Array, T Min, T Max);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampInteger", Keywords = "Clamp plugin Array Integer", ToolTip = "Clamp an array"), Category = "Clamp")
    static TArray<int32> ClampInteger(TArray<int32> Array, int32 Min, int32 Max);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampFloat", Keywords = "Clamp plugin Array Float", ToolTip = "Clamp an array"), Category = "Clamp")
    static TArray<float> ClampFloat(TArray<float> Array, float Min, float Max);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampVector", Keywords = "Clamp plugin Array Vector Size", ToolTip = "Clamp an array"), Category = "Clamp")
    static TArray<FVector> ClampVectorSize(TArray<FVector> Array, float MinSize, float MaxSize, bool bOnly2D = false);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampIntegerByRef", Keywords = "Clamp plugin Array Integer Reference", ToolTip = "Clamp an array"), Category = "Clamp")
    static void ClampIntegerRef(UPARAM(ref) TArray<int32>& Array, int32 Min, int32 Max);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampFloatByRef", Keywords = "Clamp plugin Array Float Reference", ToolTip = "Clamp an array"), Category = "Clamp")
    static void ClampFloatRef(UPARAM(ref) TArray<float>& Array, float Min, float Max);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampVectorRef", Keywords = "Clamp plugin Array Vector Size Reference", ToolTip = "Clamp an array"), Category = "Clamp")
    static void ClampVectorSizeRef(UPARAM(ref) TArray<FVector>& Array, float MinSize, float MaxSize, bool bOnly2D = false);

#pragma endregion

#pragma region Range

    template <typename T>
    static TArray<T> Extract(const TArray<T>& Array, int32 StartIndex, int32 EndIndex);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ExtractInteger", Keywords = "Extract plugin Array Integer Subarray", ToolTip = "Extracts a subarray from array"), Category = "Range")
    static TArray<int32> ExtractInteger(const TArray<int32>& Array, int32 StartIndex, int32 EndIndex);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ExtractFloat", Keywords = "Extract plugin Array Float Subarray", ToolTip = "Extracts a subarray from array"), Category = "Range")
    static TArray<float> ExtractFloat(const TArray<float>& Array, int32 StartIndex, int32 EndIndex);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ExtractString", Keywords = "Extract plugin Array String Subarray", ToolTip = "Extracts a subarray from array"), Category = "Range")
    static TArray<FString> ExtractString(const TArray<FString>& Array, int32 StartIndex, int32 EndIndex);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ExtractName", Keywords = "Extract plugin Array Name Subarray", ToolTip = "Extracts a subarray from array"), Category = "Range")
    static TArray<FName> ExtractName(const TArray<FName>& Array, int32 StartIndex, int32 EndIndex);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ExtractVector", Keywords = "Extract plugin Array Vector Subarray", ToolTip = "Extracts a subarray from array"), Category = "Range")
    static TArray<FVector> ExtractVector(const TArray<FVector>& Array, int32 StartIndex, int32 EndIndex);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ExtractActor", Keywords = "Extract plugin Array Actor Subarray", ToolTip = "Extracts a subarray from array"), Category = "Range")
    static TArray<AActor*> ExtractActor(const TArray<AActor*>& Array, int32 StartIndex, int32 EndIndex);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ExtractObject", Keywords = "Extract plugin Array Object Subarray", ToolTip = "Extracts a subarray from array"), Category = "Range")
    static TArray<UObject*> ExtractObject(const TArray<UObject*>& Array, int32 StartIndex, int32 EndIndex);

#pragma endregion

#pragma region Random

    template <typename T>
    static TArray<T> Random(int32 Size, T& MinValue, T& MaxValue);
    UFUNCTION(BlueprintPure, meta = (DisplayName = "RandomInteger", Keywords = "Random plugin Array Integer", ToolTip = "Generates a random array"), Category = "Random")
    static TArray<int32> RandomInteger(int32 Size, int32 MinValue, int32 MaxValue);
    UFUNCTION(BlueprintPure, meta = (DisplayName = "RandomFloat", Keywords = "Random plugin Array Float", ToolTip = "Generates a random array"), Category = "Random")
    static TArray<float> RandomFloat(int32 Size, float MinValue, float MaxValue);
    UFUNCTION(BlueprintPure, meta = (DisplayName = "RandomVector", Keywords = "Random plugin Array Vector", ToolTip = "Generates a random array"), Category = "Random")
    static TArray<FVector> RandomVector(int32 Size, FVector MinValue, FVector MaxValue);

#pragma endregion

#pragma region Sorts

    template <typename T>
    static TArray<T> Sort(TArray<T>& Array, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortInteger", Keywords = "Sort plugin Array Integer", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
    static TArray<int32> SortInteger(TArray<int32> Array, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortFloat", Keywords = "Sort plugin Array Float", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
    static TArray<float> SortFloat(TArray<float> Array, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortString", Keywords = "Sort plugin Array String", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
    static TArray<FString> SortString(TArray<FString> Array, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortName", Keywords = "Sort plugin Array Name", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
    static TArray<FName> SortName(TArray<FName> Array, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortActor", Keywords = "Sort plugin Array Actor", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
    static TArray<AActor*> SortActor(TArray<AActor*> Array, AActor* const& Actor, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortVector", Keywords = "Sort plugin Array Vector", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
    static TArray<FVector> SortVector(TArray<FVector> Array, FVector Origin, bool bIsAscending = true);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortIntegerByRef", Keywords = "Sort plugin Array Integer Reference", ToolTip = "Sort an array in ascending or descending order by reference"),
        Category = "Sort")
    static void SortIntegerRef(UPARAM(ref) TArray<int32>& Array, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortFloatByRef", Keywords = "Sort plugin Array Float Reference", ToolTip = "Sort an array in ascending or descending order by reference"),
        Category = "Sort")
    static void SortFloatRef(UPARAM(ref) TArray<float>& Array, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortStringByRef", Keywords = "Sort plugin Array String Reference", ToolTip = "Sort an array in ascending or descending order by reference"),
        Category = "Sort")
    static void SortStringRef(UPARAM(ref) TArray<FString>& Array, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortNameByRef", Keywords = "Sort plugin Array Name Reference", ToolTip = "Sort an array in ascending or descending order by reference"),
        Category = "Sort")
    static void SortNameRef(UPARAM(ref) TArray<FName>& Array, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortActorByRef", Keywords = "Sort plugin Array Actor Reference", ToolTip = "Sort an array in ascending or descending order by reference"),
        Category = "Sort")
    static void SortActorRef(UPARAM(ref) TArray<AActor*>& Array, AActor* const& Actor, bool bIsAscending = true);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortVectorByRef", Keywords = "Sort plugin Array Vector Reference", ToolTip = "Sort an array in ascending or descending order by reference"),
        Category = "Sort")
    static void SortVectorRef(UPARAM(ref) TArray<FVector>& Array, FVector Origin, bool bIsAscending = true);

#pragma endregion

#pragma region SortPredicate

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortInteger", Keywords = "Sort plugin Array Integer Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
    static TArray<int32> PredicateSortInteger(TArray<int32> Array, const FIntSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortFloat", Keywords = "Sort plugin Array Float Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
    static TArray<float> PredicateSortFloat(TArray<float> Array, const FFloatSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortString", Keywords = "Sort plugin Array String Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
    static TArray<FString> PredicateSortString(TArray<FString> Array, const FStringSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortName", Keywords = "Sort plugin Array Name Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
    static TArray<FName> PredicateSortName(TArray<FName> Array, const FNameSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortVector", Keywords = "Sort plugin Array Vector Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
    static TArray<FVector> PredicateSortVector(TArray<FVector> Array, const FVectorSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortActor", Keywords = "Sort plugin Array Actor Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
    static TArray<AActor*> PredicateSortActor(TArray<AActor*> Array, const FActorSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortObject", Keywords = "Sort plugin Array Object Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
    static TArray<UObject*> PredicateSortObject(TArray<UObject*> Array, const FObjectSortDelegate& PredicateFunction, bool InvertResult = false);

    UFUNCTION(BlueprintCallable,
        meta = (DisplayName = "PredicateSortIntegerByRef", Keywords = "Sort plugin Array Integer Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
    static void PredicateSortIntegerRef(UPARAM(ref) TArray<int32>& Array, const FIntSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortFloatByRef", Keywords = "Sort plugin Array Float Predicate Reference", ToolTip = "Sort an array using a predicate by reference"),
        Category = "Sort")
    static void PredicateSortFloatRef(UPARAM(ref) TArray<float>& Array, const FFloatSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortStringByRef", Keywords = "Sort plugin Array String Predicate Reference", ToolTip = "Sort an array using a predicate by reference"),
        Category = "Sort")
    static void PredicateSortStringRef(UPARAM(ref) TArray<FString>& Array, const FStringSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortNameByRef", Keywords = "Sort plugin Array Name Predicate Reference", ToolTip = "Sort an array using a predicate by reference"),
        Category = "Sort")
    static void PredicateSortNameRef(UPARAM(ref) TArray<FName>& Array, const FNameSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortVectorByRef", Keywords = "Sort plugin Array Vector Predicate Reference", ToolTip = "Sort an array using a predicate by reference"),
        Category = "Sort")
    static void PredicateSortVectorRef(UPARAM(ref) TArray<FVector>& Array, const FVectorSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortActorByRef", Keywords = "Sort plugin Array Actor Predicate Reference", ToolTip = "Sort an array using a predicate by reference"),
        Category = "Sort")
    static void PredicateSortActorRef(UPARAM(ref) TArray<AActor*>& Array, const FActorSortDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortObjectByRef", Keywords = "Sort plugin Array Object Predicate Reference", ToolTip = "Sort an array using a predicate by reference"),
        Category = "Sort")
    static void PredicateSortObjectRef(UPARAM(ref) TArray<UObject*>& Array, const FObjectSortDelegate& PredicateFunction, bool InvertResult = false);

#pragma endregion

#pragma region Distance

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClosestLocation", Keywords = "Vector plugin Array Closest", ToolTip = "Get closest location to an origin"), Category = "Distance")
    static void ClosestLocation(const TArray<FVector>& Array, FVector Origin, FVector& Closest, float& Distance, int32& Index);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "FarthestLocation", Keywords = "Vector plugin Array Farthest", ToolTip = "Get farthest location to an origin"), Category = "Distance")
    static void FarthestLocation(const TArray<FVector>& Array, FVector Origin, FVector& Farthest, float& Distance, int32& Index);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClosestActor", Keywords = "Actor plugin Array Closest", ToolTip = "Get closest actor to an origin actor"), Category = "Distance")
    static void ClosestActor(const TArray<AActor*>& Array, AActor* const& Origin, AActor*& Closest, float& Distance, int32& Index);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "FarthestActor", Keywords = "Actor plugin Array Farthest", ToolTip = "Get farthest actor to an origin actor"), Category = "Distance")
    static void FarthestActor(const TArray<AActor*>& Array, AActor* const& Origin, AActor*& Farthest, float& Distance, int32& Index);

#pragma endregion

#pragma region Filters

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "FilterMatches", Keywords = "Filter plugin Array Matches Regex", ToolTip = "Finds matching regex expressions in array"), Category = "Filter")
    static TArray<FString> FilterMatches(const TArray<FString>& Array, const FString& Pattern, bool& bFound, TArray<int32>& Indexes);
    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "FilterMatch", Keywords = "Filter plugin Array Match Regex", ToolTip = "Finds the first matching regex expression in array"), Category = "Filter")
    static FString FilterMatch(const TArray<FString>& Array, const FString& Pattern, bool& bFound, int32& Index);

#pragma endregion

#pragma region FiltersPredicate

    UFUNCTION(
        BlueprintCallable, meta = (DisplayName = "PredicateFilterInteger", Keywords = "Filter plugin Array Integer Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
    static TArray<int32> PredicateFilterInteger(const TArray<int32>& Array, const FIntFilterDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterFloat", Keywords = "Filter plugin Array Float Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
    static TArray<float> PredicateFilterFloat(const TArray<float>& Array, const FFloatFilterDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterString", Keywords = "Filter plugin Array String Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
    static TArray<FString> PredicateFilterString(const TArray<FString>& Array, const FStringFilterDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterName", Keywords = "Filter plugin Array Name Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
    static TArray<FName> PredicateFilterName(const TArray<FName>& Array, const FNameFilterDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterVector", Keywords = "Filter plugin Array Vector Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
    static TArray<FVector> PredicateFilterVector(const TArray<FVector>& Array, const FVectorFilterDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterActor", Keywords = "Filter plugin Array Actor Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
    static TArray<AActor*> PredicateFilterActor(const TArray<AActor*>& Array, const FActorFilterDelegate& PredicateFunction, bool InvertResult = false);
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterObject", Keywords = "Filter plugin Array Object Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
    static TArray<UObject*> PredicateFilterObject(const TArray<UObject*>& Array, const FObjectFilterDelegate& PredicateFunction, bool InvertResult = false);

#pragma endregion

#pragma endregion

#pragma region ActionMath

#pragma region MathEasing

public:
    /**
     * Determine the current easing value of alpha (See https://easings.net/ for visual output examples)
     * @param Easing - The easing to use
     * @param Alpha - The alpha, clamped 0-1
     * @return The easing value, see https://easings.net/ to see visually what values between 0 and 1 will produce.
     */
    UFUNCTION(BlueprintPure, Category = "MathEasing")
    static float EaseFloat(const ERyMathEasingType Easing = ERyMathEasingType::Linear, float Alpha = 0.0f);

    /**
     * Ease a vector from start to target
     * @param Easing - The easing to use
     * @param Start - The starting vector
     * @param Target - The target vector
     * @param Alpha - The alpha, clamped 0-1
     * @return The vector eased from start to target depending on the alpha
     */
    UFUNCTION(BlueprintPure, Category = "MathEasing")
    static FVector EaseVector(const ERyMathEasingType Easing, const FVector& Start, const FVector& Target, const float Alpha = 0.0f);

    /**
     * Ease a rotator from start to target
     * @param Easing - The easing to use
     * @param Start - The starting rotator
     * @param Target - The target rotator
     * @param Alpha - The alpha, clamped 0-1
     * @return The rotator eased from start to target depending on the alpha
     */
    UFUNCTION(BlueprintPure, Category = "MathEasing")
    static FRotator EaseRotator(const ERyMathEasingType Easing, const FRotator& Start, const FRotator& Target, const float Alpha = 0.0f);

#pragma endregion

#pragma region Angle

public:
    /** @public Angle in radians between VectorA and VectorB. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Angle")
    static float GetAngleInRadiansBetweenVectors(const FVector& VectorA, const FVector& VectorB);

    /** @public Angle in degrees(-180 to 180) between VectorA and VectorB. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Angle")
    static float GetAngleBetweenVectors(const FVector& VectorA, const FVector& VectorB);

    /** @public Angle in degrees(-180 to 180) between Actor A and Actor B. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Angle")
    static float GetAngleBetweenActors(AActor* A, AActor* B);

#pragma endregion

#pragma region Console

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Console")
    static float GetConsoleVariableFloat(const FString& VariableName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Console")
    static int32 GetConsoleVariableInt(const FString& VariableName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Console")
    static bool GetConsoleVariableBool(const FString& VariableName);

#pragma endregion

#pragma region Numeric

public:
    /** @public Rounds an integer to the lower multiple of the given number.
        If Skip Self is set to True it will skip to the previous multiple if the integer rounds to itself.
        @param Multiple - The multiple number to round to.
        @param skipSelf - Skip to the previous multiple if the integer rounds to itself.
    **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Numeric")
    static int32 RoundToLowerMultiple(int32 A, int32 Multiple = 32, bool SkipSelf = false);

    /** @public Rounds an integer to the upper multiple of the given number.
        If Skip Self is set to True it will skip to the next multiple if the integer rounds to itself.
        @param Multiple - The multiple number to round to.
        @param skipSelf - Skip to the next multiple if the integer rounds to itself.
    **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Numeric")
    static int32 RoundToUpperMultiple(int32 A, int32 Multiple = 32, bool SkipSelf = false);

    /** @public Rounds an integer to the nearest multiple of the given number. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Numeric")
    static int32 RoundToNearestMultiple(int32 A, int32 Multiple = 32);

    /** @public true if the integer is a power of two number. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "PwrOf2"), Category = "Numeric")
    static bool IsPowerOfTwo(int32 x);

    /** @public true if the integer is a multiple of the given number. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Numeric")
    static bool IsMultipleOf(int32 A, int32 Multiple);

    /** @public true if the number is even (false if it's odd). **/
    UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "isEven"), Category = "Numeric")
    static bool IsEvenNumber(float A);

#pragma endregion

#pragma region Vector

public:
    /** @public Find closest point on a Sphere to a Line.
        When line intersects Sphere, then closest point to LineOrigin is returned.
        @param SphereOrigin		Origin of Sphere
        @param SphereRadius		Radius of Sphere
        @param LineOrigin		Origin of line
        @param LineDir			Direction of line.
        @return Closest point on sphere to given line.
    **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vector")
    static FVector ClosestPointOnSphereToLine(const FVector& SphereOrigin, float SphereRadius, const FVector& LineOrigin, const FVector& LineDir);

    /** @public Find the point on line segment from LineStart to LineEnd which is closest to Point. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vector")
    static FVector ClosestPointOnLineSegment(const FVector& Point, const FVector& LineStart, const FVector& LineEnd);

    /** @public Returns a vector point which is a projection from a point to a line (defined by the vector couple LineOrigin, LineDirection).
        The line is infinite (in both directions). **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vector")
    static FVector ProjectPointOnLine(const FVector& LineOrigin, const FVector& LineDirection, const FVector& Point);

    /** @public Find closest points between 2 line segments.
        @param	(Line1Start, Line1End)	defines the first line segment.
        @param	(Line2Start, Line2End)	defines the second line segment.
        @param	LinePoint1		Closest point on segment 1 to segment 2.
        @param	LinePoint2		Closest point on segment 2 to segment 1. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vector")
    static void ClosestPointsOfLineSegments(const FVector& Line1Start, const FVector& Line1End, const FVector& Line2Start, const FVector& Line2End, FVector& LinePoint1, FVector& LinePoint2);

    /** @public Calculate the closest points between two infinitely long lines.
        If lines are intersecting (not parallel) it will return false! Use Line To Line Intersection instead.
        @param closestPointLine1 The closest point of line1 to line2. Returns zero if the lines intersect.
        @param closestPointLine2 The closest point of line2 to line1. Returns zero if the lines intersect.
        @param linePoint1 Line point of the first line.
        @param lineVec1 Line direction (normal) of the first line. Should be a normalized vector.
        @param linePoint2 Line point of the second line.
        @param lineVec2 Line direction (normal) of the second line. Should be a normalized vector.
        @return true if lines are parallel, false otherwise. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vector")
    static bool ClosestPointsOnTwoLines(const FVector& LinePoint1, const FVector& LineVec1, const FVector& LinePoint2, const FVector& LineVec2, FVector& ClosestPointLine1, FVector& ClosestPointLine2);

    /** @public Snaps vector to nearest grid multiple. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vector")
    static FVector GridSnap(const FVector& A, float Grid);

    /** @public Converts Physics Linear Velocity to a more useful speed unit. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vector")
    static float ConvertPhysicsLinearVelocity(const FVector& Velocity, ESpeedUnit SpeedUnit);

    /** @public Returns true if the given Point vector is within a box (defined by BoxOrigin and BoxExtent). **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vector")
    static bool IsPointInsideBox(const FVector& Point, const FVector& BoxOrigin, const FVector& BoxExtent);

    /** @public Determines whether a line intersects a sphere. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vector")
    static bool IsLineInsideSphere(const FVector& LineStart, const FVector& LineDir, double LineLength, const FVector& SphereOrigin, double SphereRadius);

#pragma endregion

#pragma region Physics

public:
    /** @public Get the current world velocity of a point on a physics-enabled component.
        @param Point - Point in world space.
        @param DrawDebugInfo - Draw debug point & string.
        @param BoneName - If a SkeletalMeshComponent, name of body to get velocity of. 'None' indicates root body.
        @return The velocity of the point in world space. **/
    UFUNCTION(BlueprintCallable, Category = "Physics")
    static FVector GetVelocityAtPoint(UPrimitiveComponent* Target, const FVector& Point, FName BoneName = NAME_None);

    /** @public Set the physx center of mass offset.
        Note: this offsets the physx-calculated center of mass (it is not an offset from the pivot point). **/
    UFUNCTION(BlueprintCallable, Category = "Physics")
    static void SetCenterOfMassOffset(UPrimitiveComponent* Target, const FVector& Offset, FName BoneName = NAME_None);

#pragma endregion

#pragma region Random

public:
    /** @public Vector which will be in range between Start->End **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Random")
    static FVector RandomVectorInRange(const FVector& Start, const FVector& End);

    /** @public Generate string from Alphabet.
        @param Alphabet - Array of available letters in string
        @param MinLettersCount - Minimal count of letters in string
        @param MaxLettersCount - Maximum count of letters in string
        @return Generated string. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Random")
    static FString RandomStringFromAlphabet(const TArray<FString>& Alphabet, int32 MinLettersCount, int32 MaxLettersCount);

    /** @public Generate color between two colors. **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Random")
    static FColor RandomColorInRange(const FColor& StartColor, const FColor& EndColor);

    /** @public Generate random integer array. **/
    UFUNCTION(BlueprintCallable, Category = "Random")
    static void RandomIntArray(int32 ArrayLength, int32 MinValue, int32 MaxValue, TArray<int32>& RandomArray);

#pragma endregion

#pragma endregion

#pragma region ActionWorld

public:
    /**
     * @public Set a timer to execute delegate. Setting an existing timer will reset that timer with updated parameters.
     * @param Event						Event. Can be a K2 function or a Custom Event.
     * @param Time						How long to wait before executing the delegate, in seconds. Setting a timer to <= 0 seconds will clear it if it is set.
     * @param bLooping					True to keep executing the delegate every Time seconds, false to execute delegate only once.
     * @param InitialStartDelay			Initial delay passed to the timer manager, in seconds.
     * @param InitialStartDelayVariance	Use this to add some variance to when the timer starts in lieu of doing a random range on the InitialStartDelay input, in seconds.
     * @return							The timer handle to pass to other timer functions to manipulate this timer.
     */
    UFUNCTION(BlueprintCallable, Category = "World", meta = (WorldContext = "WorldContextObject"))
    static FTimerHandle SetTimerByEventForWorld(
        UObject* WorldContextObject, UPARAM(DisplayName = "Event") FTimerDynamicDelegate Delegate, float Time, bool bLooping, float InitialStartDelay = 0.f, float InitialStartDelayVariance = 0.f);

    /**
     * @public Set a timer to execute delegate. Setting an existing timer will reset that timer with updated parameters.
     * @param Object					Object that implements the delegate function. Defaults to self (this blueprint)
     * @param FunctionName				Delegate function name. Can be a K2 function or a Custom Event.
     * @param Time						How long to wait before executing the delegate, in seconds. Setting a timer to <= 0 seconds will clear it if it is set.
     * @param bLooping					true to keep executing the delegate every Time seconds, false to execute delegate only once.
     * @param InitialStartDelay			Initial delay passed to the timer manager to allow some variance in when the timer starts, in seconds.
     * @param InitialStartDelayVariance	Use this to add some variance to when the timer starts in lieu of doing a random range on the InitialStartDelay input, in seconds.
     * @return							The timer handle to pass to other timer functions to manipulate this timer.
     */
    UFUNCTION(BlueprintCallable, Category = "World", meta = (WorldContext = "WorldContextObject"))
    static FTimerHandle SetTimerByFunctionForWorld(
        UObject* WorldContextObject, UBlueprint* Object, FString FunctionName, float Time, bool bLooping, float InitialStartDelay = 0.f, float InitialStartDelayVariance = 0.f);

    /** @public A quick way to create a timer **/
    template <class UserClass>
    static FTimerHandle QuickTimer(UWorld* World, UserClass* InObj, typename FTimerDelegate::TMethodPtr<UserClass> InTimerMethod, float Timer, bool bLooping)
    {
        if (World && InObj)
        {
            FTimerHandle TimerHandle;
            World->GetTimerManager().SetTimer(TimerHandle, InObj, InTimerMethod, Timer, bLooping);
            return TimerHandle;
        }
        return {};
    }

    /** @public A quick way to create a timer for next tick **/
    template <class UserClass>
    static void QuickTimerNextTick(UWorld* World, UserClass* InObj, typename FTimerDelegate::TMethodPtr<UserClass> InTimerMethod)
    {
        if (World && InObj)
        {
            World->GetTimerManager().SetTimerForNextTick(InObj, InTimerMethod);
        }
    }

    /** @public A quick way to create a timer by timer delegate **/
    static FTimerHandle QuickTimerByTimerDelegate(const UWorld* World, const FTimerDelegate& TimerDelegate, float Timer, bool bLooping);

    /** @public A quick way to create a timer for next tick by timer delegate **/
    static void QuickTimerNextTickByTimerDelegate(const UWorld* World, const FTimerDelegate& TimerDelegate);

    /** @public Cleaning the timer structure **/
    static void ClearTimerHandle(const UWorld* World, FTimerHandle& TimerHandle);

    /** @public Checking the validity of the structure FTimerHandle **/
    static bool CheckValidTimerHandle(const UWorld* World, const FTimerHandle& TimerHandle);

#pragma endregion

#pragma region ActionStr

public:
    /**
     * Is the name None?
     */
    UFUNCTION(BlueprintPure, Category = "StrHelper")
    static bool IsNone(const FName Name);

    /**
     * Is the string empty? (Has no characters)
     */
    UFUNCTION(BlueprintPure, Category = "StrHelper")
    static bool IsEmpty(const FString& String);

    /**
     * Sets a char by index in the string
     */
    UFUNCTION(BlueprintCallable, Category = "StrHelper")
    static void SetChar(UPARAM(ref) FString& SourceString, const int32 CharIndex, const FString& CharIn);

    /**
     * Gets a char by index in the string
     */
    UFUNCTION(BlueprintPure, Category = "StrHelper")
    static FString GetChar(UPARAM(ref) FString& SourceString, const int32 CharIndex);

    /**
     * Pops a character off the end of a string
     */
    UFUNCTION(BlueprintCallable, Category = "StrHelper")
    static void PopChar(UPARAM(ref) FString& SourceString);

    /**
     * Pushes a character to the end of a string
     */
    UFUNCTION(BlueprintCallable, Category = "StrHelper")
    static void PushChar(UPARAM(ref) FString& SourceString, const FString& CharIn);

    /**
     * Fills a string with a specific character overwriting the original string. Good for initializing a string with characters to start.
     */
    UFUNCTION(BlueprintCallable, Category = "StrHelper")
    static void FillString(UPARAM(ref) FString& SourceString, const int32 NumChars, const FString& CharIn);

    /**
     * Sets the reserved number of characters in a string.
     */
    UFUNCTION(BlueprintCallable, Category = "StrHelper")
    static void SetReserved(UPARAM(ref) FString& SourceString, const int32 ReserveNumChars);

    /**
     * Append a string to a source string
     */
    UFUNCTION(BlueprintCallable, Category = "StrHelper")
    static void AppendTo(UPARAM(ref) FString& SourceString, const FString& Appendage);

    /**
     * Append many strings to a source string
     */
    UFUNCTION(BlueprintCallable, Category = "StrHelper")
    static void AppendStringsTo(UPARAM(ref) FString& SourceString, const TArray<FString>& Appendages);

    /**
     * Returns the bytes representing a character in a string. Two bytes to represent a UTF-16 TCHAR.
     * If TCHAR is 8bits on this system, byte2 will contain the character, byte1 will be zero.
     */
    UFUNCTION(BlueprintPure, Category = "StrHelper")
    static void CharToBytes(const FString& sourceString, const int32 charIndex, uint8& byte1, uint8& byte2);

    /// Returns a string where the first character in every word is upper case. Like a header, or a title.
    UFUNCTION(BlueprintCallable, Category = "StrHelper")
    static void ToTitleStringInline(UPARAM(ref) FString& InOutString);

    /// Returns a string where the first character in every word is upper case. Like a header, or a title.
    UFUNCTION(BlueprintPure, Category = "StrHelper")
    static FString ToTitleString(const FString& InString);

    /// Similar to MakeTextFromStringTable but instead of whining about a missing entry, sets isValid to false if no string exists or true if one does, and returns the text
    /// which is empty if there is no entry, or populated if there is.
    /// Also, takes in a string table asset instead of a table ID.
    /// @param Table     The table to look in
    /// @param Key       The key into the string table to find the text
    /// @param bIsValid   True if the text was found and set to the return of the function
    /// @return The text if it was found
    UFUNCTION(BlueprintCallable, Category = "StrHelper", meta = (DisplayName = "Get Text From String Table (Advanced)"))
    static FText GetTextFromStringTable(class UStringTable* Table, const FString& Key, bool& bIsValid);

    /// Similar to MakeTextFromStringTable but instead of whining about a missing entry, sets isValid to false if no string exists or true if one does, and returns the text
    /// which is empty if there is no entry, or populated if there is.
    /// Try to use GetTextFromStringTable instead as it should help have the asset pre-loaded. If you call GetTextFromStringTableID
    /// while the string table isn't loaded, it will try to syncronously load the string table asset.
    /// @param TableID   The table to look up
    /// @param Key       The key into the string table to find the text
    /// @param bIsValid   True if the text was found and set to the return of the function
    /// @return The text if it was found
    UFUNCTION(BlueprintCallable, Category = "StrHelper", meta = (DisplayName = "Get Text From String Table ID (Advanced)"))
    static FText GetTextFromStringTableID(FName TableID, const FString& Key, bool& bIsValid);

    /** Converts a int64 value to a string */
    UFUNCTION(BlueprintPure, Category = "StrHelper", meta = (DisplayName = "ToString (Int64)", CompactNodeTitle = "->", BlueprintAutocast))
    static FString Conv_Int64ToString(int64 InInt64);

    /** Converts a TArray<uint8> value to a string */
    UFUNCTION(BlueprintPure, Category = "StrHelper", meta = (DisplayName = "ToString (ArrayUint8)", CompactNodeTitle = "->", BlueprintAutocast))
    static FString Conv_ArrayUint8ToString(const TArray<uint8>& ArrayUint8);

    /** Combines file paths */
    UFUNCTION(BlueprintPure, Category = "StrHelper", meta = (CommutativeAssociativeBinaryOperator = "true"))
    static FString CombineFilePaths(FString A, FString B);

#pragma endregion

#pragma region ActionWidget

public:
    /** @public Getting the mouse position on the widget **/
    UFUNCTION(BlueprintPure, Category = "Widgets")
    static FVector2D GetMousePositionByWidget(UWidget* Widget);

    /** @public Getting UV coordinates by mouse position **/
    UFUNCTION(BlueprintPure, Category = "Widgets")
    static FVector2D GetMouseUVPosition(UWidget* Widget);

    /** @public Getting UV inverted coordinates by mouse position **/
    UFUNCTION(BlueprintPure, Category = "Widgets")
    static FVector2D GetMouseUVPositionInvert(UWidget* Widget);

#pragma endregion

#pragma region ActionJSON

public:
    /** @public Creates a json object
     * @return	The json object */
    UFUNCTION(BlueprintPure, Category = "Json", meta = (NativeMakeFunc))
    static FBlueprintJsonObject JsonMake();

    /**
     * @public Sets the value of the field with the specified name.
     *
     * @param	JsonObject	The stored json object
     * @param	FieldName	The name of the field to set.
     * @param	Value		The json value to set.
     * @return	The stored json object
     */
    UFUNCTION(BlueprintPure, Category = "Json")
    static const FBlueprintJsonObject& JsonMakeField(const FBlueprintJsonObject& JsonObject, const FString& FieldName, const FBlueprintJsonValue& Value);

    /**
     * @public Sets the value of the field with the specified name.
     *
     * @param	JsonObject	The stored json object
     * @param	FieldName	The name of the field to set.
     * @param	Value		The json value to set.
     * @return	The stored json object
     */
    UFUNCTION(BlueprintCallable, Category = "Json")
    static const FBlueprintJsonObject& JsonSetField(const FBlueprintJsonObject& JsonObject, const FString& FieldName, const FBlueprintJsonValue& JsonValue);

    /**
     * @public Checks whether a field with the specified name exists in the json object.
     *
     * @param	JsonObject	The stored json object
     * @param	FieldName	The name of the field to check.
     * @return true if the field exists, false otherwise.
     */
    UFUNCTION(BlueprintPure, Category = "Json")
    static bool JsonHasField(const FBlueprintJsonObject& JsonObject, const FString& FieldName);

    /**
     * @public Checks whether a field with the specified name and type exists in the object.
     *
     * @param	JsonObject	The stored json object
     * @param	FieldName	The name of the field to check.
     * @param	Type		The type of the field to check.
     * @return true if the field exists, false otherwise.
     */
    UFUNCTION(BlueprintPure, Category = "Json")
    static bool JsonHasTypedField(const FBlueprintJsonObject& JsonObject, const FString& FieldName, EJsonType Type);

    /**
     * @public Removes the field with the specified name.
     *
     * @param	JsonObject	The stored json object
     * @param	FieldName	The name of the field to remove.
     * @return	The stored json object
     */
    UFUNCTION(BlueprintCallable, Category = "Json")
    static const FBlueprintJsonObject& JsonRemoveField(const FBlueprintJsonObject& JsonObject, const FString& FieldName);

    /**
     * @public Convert json object to json string
     *
     * @param	JsonObject	The stored json object
     * @param	FieldName	The name of the field to get.
     * @return	The json value of json object
     */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToJsonValue (JsonObject)", CompactNodeTitle = "ToValue", BlueprintAutocast, NativeBreakFunc), Category = "Json|Convert")
    static FBlueprintJsonValue Conv_JsonObjectToJsonValue(const FBlueprintJsonObject& JsonObject, const FString& FieldName);

    /**
     * @public Convert json object to json string
     *
     * @param	JsonObject	The json object to convert
     * @return	The json string
     */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (JsonObject)", CompactNodeTitle = "ToString", BlueprintAutocast, NativeBreakFunc), Category = "Json|Convert")
    static FString Conv_JsonObjectToString(const FBlueprintJsonObject& JsonObject);

    /**
     * @public Convert json object to pretty print json string
     *
     * @param	JsonObject	The json object to convert
     * @return	The json string
     */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToPrettyString (JsonObject)", CompactNodeTitle = "ToPrettyString", NativeBreakFunc), Category = "Json|Convert")
    static FString Conv_JsonObjectToPrettyString(const FBlueprintJsonObject& JsonObject);

    /**
     * @public Convert json string to json object
     *
     * @param	JsonString	The string to convert
     * @return	The json object
     */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToJsonObject (String)", CompactNodeTitle = "ToJson"), Category = "Json|Convert")
    static FBlueprintJsonObject Conv_StringToJsonObject(const FString& JsonString);

    /**
     * @public Creates a json value string
     *
     * @param	Value	value to set the string to
     * @return	The blueprint json value
     */
    UFUNCTION(BlueprintPure, Category = "Json|Make", meta = (NativeMakeFunc))
    static FBlueprintJsonValue JsonMakeString(const FString& StringValue);

    /**
     * @public Creates a json value int
     *
     * @param	Value	value to set the int to
     * @return	The blueprint json value
     */
    UFUNCTION(BlueprintPure, Category = "Json|Make", meta = (NativeMakeFunc))
    static FBlueprintJsonValue JsonMakeInt(int32 IntValue);

    /**
     * @public Creates a json value float
     *
     * @param	Value	value to set the float to
     * @return	The blueprint json value
     */
    UFUNCTION(BlueprintPure, Category = "Json|Make", meta = (NativeMakeFunc))
    static FBlueprintJsonValue JsonMakeFloat(float FloatValue);

    /**
     * @public Creates a json value bool

     * @param	Value	value to set the bool to
     * @return	The blueprint json value
     */
    UFUNCTION(BlueprintPure, Category = "Json|Make", meta = (NativeMakeFunc))
    static FBlueprintJsonValue JsonMakeBool(bool BoolValue);

    /**
     * @public Creates a json value array
     *
     * @param	Value	value to set the array to
     * @return	The blueprint json value
     */
    UFUNCTION(BlueprintPure, Category = "Json|Make", meta = (NativeMakeFunc))
    static FBlueprintJsonValue JsonMakeArray(const TArray<FBlueprintJsonValue>& ArrayValue);

    /**
     * @public Creates a json value object
     *
     * @param	Value	value to set the json object to
     * @return	The blueprint json value
     */
    UFUNCTION(BlueprintPure, Category = "Json|Make", meta = (NativeMakeFunc))
    static FBlueprintJsonValue JsonMakeObject(const FBlueprintJsonObject& ObjectValue);

    /**
     * @public Creates a json value null
     *
     * @return	The blueprint json value
     */
    UFUNCTION(BlueprintPure, Category = "Json|Make", meta = (NativeMakeFunc))
    static FBlueprintJsonValue JsonMakeNull();

    /** @public Return the type of json value */
    UFUNCTION(BlueprintPure, Category = "Json|Value")
    static EJsonType JsonType(const FBlueprintJsonValue& JsonValue);

    /** @public Return true if the json value is null, false otherwise */
    UFUNCTION(BlueprintPure, Category = "Json|Value")
    static bool JsonIsNull(const FBlueprintJsonValue& JsonValue);

    /** @public Returns true if the values are equal (A == B) */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (JsonValue)", CompactNodeTitle = "=="), Category = "Json|Value")
    static bool EquaEqual_JsonValue(const FBlueprintJsonValue& A, const FBlueprintJsonValue& B);

    /** @public Returns true if the values are not equal (A != B) */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (JsonValue)", CompactNodeTitle = "!="), Category = "Json|Value")
    static bool NotEqual_JsonValue(const FBlueprintJsonValue& A, const FBlueprintJsonValue& B);

    /** @public Converts an json value into an string */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast, NativeBreakFunc), Category = "Json|Value")
    static FString Conv_JsonValueToString(const FBlueprintJsonValue& JsonValue);

    /** @public Converts an json value into an int */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToInteger (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast, NativeBreakFunc), Category = "Json|Value")
    static int Conv_JsonValueToInteger(const FBlueprintJsonValue& JsonValue);

    /** @public Converts an json value into an float */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToFloat (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast, NativeBreakFunc), Category = "Json|Value")
    static float Conv_JsonValueToFloat(const FBlueprintJsonValue& JsonValue);

    /** @public Converts an json value into an bool */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToBool (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast, NativeBreakFunc), Category = "Json|Value")
    static bool Conv_JsonValueToBool(const FBlueprintJsonValue& JsonValue);

    /** @public Converts an json value into an array of json value */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToArray (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast, NativeBreakFunc), Category = "Json|Value")
    static TArray<FBlueprintJsonValue> Conv_JsonValueToArray(const FBlueprintJsonValue& JsonValue);

    /** @public Converts an json value into an json object */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToJsonObject (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast, NativeBreakFunc), Category = "Json|Value")
    static FBlueprintJsonObject Conv_JsonValueToObject(const FBlueprintJsonValue& JsonValue);

#pragma endregion
};

class FLambdaRunnable : public FRunnable
{
public:
    FLambdaRunnable(TFunction<void()> InFunction, const TCHAR* ThreadName, EThreadPriority ThreadPriority)
    {
        FunctionPointer = InFunction;
        Finished = false;

        Thread = FRunnableThread::Create(this, ThreadName, 0, ThreadPriority);  // windows default = 8mb for thread, could specify more
    }

    virtual ~FLambdaRunnable()
    {
        Thread->Kill();
        delete Thread;
        Thread = nullptr;
    }

    // Begin FRunnable interface.
    virtual bool Init() { return true; }

    virtual uint32 Run()
    {
        if (FunctionPointer)
        {
            FunctionPointer();
        }

        return 0;
    }

    virtual void Stop() { Finished = true; }

    virtual void Exit() override
    {
        Finished = true;

        // delete ourselves when we're done
        delete this;
    }

    // End FRunnable interface

    /* Makes sure this thread has stopped properly */
    void EnsureCompletion()
    {
        Stop();
        Thread->WaitForCompletion();
    }

    static FLambdaRunnable* RunLambdaOnBackGroundThread(TFunction<void()> InFunction, const TCHAR* ThreadName, EThreadPriority ThreadPriority)
    {
        FLambdaRunnable* Runnable = nullptr;
        if (FPlatformProcess::SupportsMultithreading())
        {
            Runnable = new FLambdaRunnable(InFunction, ThreadName, ThreadPriority);
        }

        return Runnable;
    }

private:
    FRunnableThread* Thread;
    TFunction<void()> FunctionPointer;
    FThreadSafeBool Finished;
};

UCLASS()
class ADVANCEGAMETOOLS_API USimpleAsync : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"),
        Category = "ActionObject")
    static USimpleAsync* SimpleAsync(UObject* WorldContextObject, EThreadTaskType ThreadType)
    {
        USimpleAsync* BlueprintNode = NewObject<USimpleAsync>();

        BlueprintNode->WorldContextObject = WorldContextObject;
        BlueprintNode->ThreadType = ThreadType;

        return BlueprintNode;
    }

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override
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

        AsyncTask(LThreadType, [&]() { DoAsync.Broadcast(); });
    }

    //~UBlueprintAsyncActionBase interface

    UPROPERTY(BlueprintAssignable)
    FSimpleAsyncCallSignature DoAsync;

private:
    const UObject* WorldContextObject;
    EThreadTaskType ThreadType;
};

UCLASS()
class ADVANCEGAMETOOLS_API UAsyncCall : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"),
        Category = "ActionObject")
    static UAsyncCall* AsyncCall(UObject* WorldContextObject, UObject* Object, const FString& FunctionName, EAsyncCallType CallType)
    {
        UAsyncCall* BlueprintNode = NewObject<UAsyncCall>();

        BlueprintNode->WorldContextObject = WorldContextObject;
        BlueprintNode->CallOnObject = Object;
        BlueprintNode->CallFunctionName = FunctionName;
        BlueprintNode->CallType = CallType;

        return BlueprintNode;
    }

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override
    {
        if (!FPlatformProcess::SupportsMultithreading() || CallOnObject == nullptr) return;

        EAsyncExecution LAsyncExecution = EAsyncExecution::Thread;

        switch (CallType)
        {
            case EAsyncCallType::MainThread:
            {
                LAsyncExecution = EAsyncExecution::TaskGraphMainThread;
                break;
            }

            case EAsyncCallType::Thread:
            {
                LAsyncExecution = EAsyncExecution::Thread;
                break;
            }

            case EAsyncCallType::ThreadPool:
            {
                LAsyncExecution = EAsyncExecution::ThreadPool;
                break;
            }
        }

        Async(LAsyncExecution,
            [&]()
            {
                FOutputDeviceNull Ar;
                CallOnObject->CallFunctionByNameWithArguments(*CallFunctionName, Ar, CallOnObject, true);
                Async(EAsyncExecution::TaskGraphMainThread, [&]() { Complete.Broadcast(); });
            });
    }

    //~UBlueprintAsyncActionBase interface

    UPROPERTY(BlueprintAssignable)
    FAsyncCallCompleteSignature Complete;

private:
    const UObject* WorldContextObject;
    UObject* CallOnObject = nullptr;
    FString CallFunctionName;
    EAsyncCallType CallType;
};

/**
 * A handle to a file
 * If this object is garbage collected or destroyed
 */
UCLASS(BlueprintType)
class ADVANCEGAMETOOLS_API UAGTFileHandle : public UObject
{
    GENERATED_BODY()

public:
    UAGTFileHandle() : Handle(nullptr), CanRead(false), CanWrite(false) {}

    virtual void BeginDestroy() override
    {
        UObject::BeginDestroy();
        if (Handle)
        {
            delete Handle;
            Handle = nullptr;
        }
    }

    /**
     * Change the current write or read position.
     */
    UFUNCTION(BlueprintCallable)
    bool Seek(const int64 newPosition)
    {
        if (!Handle)
        {
            return false;
        }

        return Handle->Seek(newPosition);
    }

    /**
     * Seek the file handle by numBytes from the end
     */
    UFUNCTION(BlueprintCallable)
    bool SeekFromEnd(const int64 numBytes)
    {
        if (!Handle)
        {
            return false;
        }

        return Handle->SeekFromEnd(numBytes);
    }

    /**
     * Seek the file handle to the start
     */
    UFUNCTION(BlueprintCallable)
    bool SeekToStart()
    {
        if (!Handle)
        {
            return false;
        }

        return Handle->Seek(0);
    }

    /**
     * Seek the file handle to the end
     */
    UFUNCTION(BlueprintCallable)
    bool SeekToEnd()
    {
        if (!Handle)
        {
            return false;
        }

        return Handle->SeekFromEnd(0);
    }

    /**
     * Read from the file
     */
    UFUNCTION(BlueprintCallable)
    bool Read(TArray<uint8>& bytesTo, const int64 numBytes)
    {
        if (!Handle || !CanRead)
        {
            return false;
        }

        const int64 numToRead = FMath::Min(Handle->Size() - Handle->Tell(), numBytes);
        if (numToRead <= 0)
        {
            return false;
        }

        bytesTo.SetNum(numToRead);
        return Handle->Read(bytesTo.GetData(), bytesTo.Num());
    }

    /**
     * Write to the file
     */
    UFUNCTION(BlueprintCallable)
    bool Write(const TArray<uint8>& bytesOut)
    {
        if (!Handle || bytesOut.Num() == 0 || !CanWrite)
        {
            return false;
        }

        return Handle->Write(bytesOut.GetData(), bytesOut.Num());
    }

    /**
     * Return the size of the file
     */
    UFUNCTION(BlueprintPure)
    int64 Size() const
    {
        if (!Handle)
        {
            return 0;
        }

        return Handle->Size();
    }

    /**
     * Closes the file handle. No further operations can be performed with this handle once this is called.
     */
    UFUNCTION(BlueprintCallable)
    void Close()
    {
        if (Handle)
        {
            delete Handle;
            Handle = nullptr;
        }
    }

private:
    // The handle to the file. If this object is destroyed the file handle will go as well.
    IFileHandle* Handle;

    bool CanRead;
    bool CanWrite;

    friend class UAdvanceGameToolLibrary;
};
