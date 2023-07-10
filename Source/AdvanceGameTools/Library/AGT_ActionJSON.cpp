/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"

#pragma region ActionJSON

typedef TSharedPtr<FJsonObject> FJsonObjectPtr;
typedef TSharedPtr<FJsonValue> FJsonValuePtr;

FBlueprintJsonObject UAdvanceGameToolLibrary::JsonMake()
{
    FBlueprintJsonObject Object;
    Object.Object = MakeShareable(new FJsonObject);
    return Object;
}

const FBlueprintJsonObject& UAdvanceGameToolLibrary::JsonMakeField(const FBlueprintJsonObject& JsonObject, const FString& FieldName, const FBlueprintJsonValue& Value)
{
    if (JsonObject.Object.IsValid() && Value.Value.IsValid())
    {
        JsonObject.Object->SetField(FieldName, Value.Value);
    }
    return JsonObject;
}

const FBlueprintJsonObject& UAdvanceGameToolLibrary::JsonSetField(const FBlueprintJsonObject& JsonObject, const FString& FieldName, const FBlueprintJsonValue& JsonValue)
{
    if (JsonObject.Object.IsValid() && JsonValue.Value.IsValid())
    {
        JsonObject.Object->SetField(FieldName, JsonValue.Value);
    }
    return JsonObject;
}

bool UAdvanceGameToolLibrary::JsonHasField(const FBlueprintJsonObject& JsonObject, const FString& FieldName)
{
    if (JsonObject.Object.IsValid())
    {
        return JsonObject.Object->HasField(FieldName);
    }
    return false;
}

bool UAdvanceGameToolLibrary::JsonHasTypedField(const FBlueprintJsonObject& JsonObject, const FString& FieldName, EJsonType Type)
{
    if (JsonObject.Object.IsValid())
    {
        if (JsonObject.Object->HasField(FieldName))
        {
            return JsonObject.Object->GetField<EJson::None>(FieldName)->Type == (EJson)Type;
        }
    }
    return false;
}

const FBlueprintJsonObject& UAdvanceGameToolLibrary::JsonRemoveField(const FBlueprintJsonObject& JsonObject, const FString& FieldName)
{
    if (JsonObject.Object.IsValid())
    {
        JsonObject.Object->RemoveField(FieldName);
    }
    return JsonObject;
}

FBlueprintJsonValue UAdvanceGameToolLibrary::Conv_JsonObjectToJsonValue(const FBlueprintJsonObject& JsonObject, const FString& FieldName)
{
    FBlueprintJsonValue Value;
    if (JsonObject.Object.IsValid())
    {
        Value.Value = JsonObject.Object->GetField<EJson::None>(FieldName);
    }
    return Value;
}

FString UAdvanceGameToolLibrary::Conv_JsonObjectToString(const FBlueprintJsonObject& JsonObject)
{
    FString Result;
    if (JsonObject.Object.IsValid())
    {
        TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Result, 0);
        FJsonSerializer::Serialize(JsonObject.Object.ToSharedRef(), JsonWriter);
    }
    return Result;
}

FString UAdvanceGameToolLibrary::Conv_JsonObjectToPrettyString(const FBlueprintJsonObject& JsonObject)
{
    FString Result;
    if (JsonObject.Object.IsValid())
    {
        TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&Result, 0);
        FJsonSerializer::Serialize(JsonObject.Object.ToSharedRef(), JsonWriter);
    }
    return Result;
}

FBlueprintJsonObject UAdvanceGameToolLibrary::Conv_StringToJsonObject(const FString& JsonString)
{
    FBlueprintJsonObject Object;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    FJsonSerializer::Deserialize(Reader, Object.Object);
    return Object;
}

FBlueprintJsonValue UAdvanceGameToolLibrary::JsonMakeString(const FString& StringValue)
{
    FBlueprintJsonValue Value;
    Value.Value = MakeShareable(new FJsonValueString(StringValue));
    return Value;
}

FBlueprintJsonValue UAdvanceGameToolLibrary::JsonMakeInt(int32 IntValue)
{
    FBlueprintJsonValue Value;
    Value.Value = MakeShareable(new FJsonValueNumber(IntValue));
    return Value;
}

FBlueprintJsonValue UAdvanceGameToolLibrary::JsonMakeFloat(float FloatValue)
{
    FBlueprintJsonValue Value;
    Value.Value = MakeShareable(new FJsonValueNumber(FloatValue));
    return Value;
}

FBlueprintJsonValue UAdvanceGameToolLibrary::JsonMakeBool(bool BoolValue)
{
    FBlueprintJsonValue Value;
    Value.Value = MakeShareable(new FJsonValueBoolean(BoolValue));
    return Value;
}

FBlueprintJsonValue UAdvanceGameToolLibrary::JsonMakeArray(const TArray<FBlueprintJsonValue>& ArrayValue)
{
    FBlueprintJsonValue Value;
    TArray<FJsonValuePtr> Array;
    for (const FBlueprintJsonValue& V : ArrayValue)
    {
        if (V.Value.IsValid())
        {
            Array.Add(V.Value);
        }
    }
    Value.Value = MakeShareable(new FJsonValueArray(Array));
    return Value;
}

FBlueprintJsonValue UAdvanceGameToolLibrary::JsonMakeObject(const FBlueprintJsonObject& ObjectValue)
{
    FBlueprintJsonValue Value;
    Value.Value = MakeShareable(new FJsonValueObject(ObjectValue.Object));
    return Value;
}

FBlueprintJsonValue UAdvanceGameToolLibrary::JsonMakeNull()
{
    FBlueprintJsonValue Value;
    Value.Value = MakeShareable(new FJsonValueNull());
    return Value;
}

EJsonType UAdvanceGameToolLibrary::JsonType(const FBlueprintJsonValue& JsonValue)
{
    if (JsonValue.Value.IsValid())
    {
        return static_cast<EJsonType>(JsonValue.Value->Type);
    }
    return EJsonType::None;
}

bool UAdvanceGameToolLibrary::JsonIsNull(const FBlueprintJsonValue& JsonValue)
{
    if (JsonValue.Value.IsValid())
    {
        return JsonValue.Value->IsNull();
    }
    return true;
}

bool UAdvanceGameToolLibrary::EquaEqual_JsonValue(const FBlueprintJsonValue& A, const FBlueprintJsonValue& B)
{
    if (A.Value.IsValid() != B.Value.IsValid())
    {
        return false;
    }

    if (A.Value.IsValid() && B.Value.IsValid())
    {
        if (!FJsonValue::CompareEqual(*A.Value, *B.Value))
        {
            return false;
        }
    }

    return true;
}

bool UAdvanceGameToolLibrary::NotEqual_JsonValue(const FBlueprintJsonValue& A, const FBlueprintJsonValue& B)
{
    if (A.Value.IsValid() != B.Value.IsValid())
    {
        return true;
    }

    if (A.Value.IsValid() && B.Value.IsValid())
    {
        if (!FJsonValue::CompareEqual(*A.Value, *B.Value))
        {
            return true;
        }
    }

    return false;
}

FString UAdvanceGameToolLibrary::Conv_JsonValueToString(const FBlueprintJsonValue& JsonValue)
{
    if (JsonValue.Value.IsValid())
    {
        return JsonValue.Value->AsString();
    }
    FString Empty;
    return Empty;
}

int UAdvanceGameToolLibrary::Conv_JsonValueToInteger(const FBlueprintJsonValue& JsonValue)
{
    if (JsonValue.Value.IsValid())
    {
        int Result = 0;
        JsonValue.Value->TryGetNumber(Result);
        return Result;
    }
    return 0;
}

float UAdvanceGameToolLibrary::Conv_JsonValueToFloat(const FBlueprintJsonValue& JsonValue)
{
    if (JsonValue.Value.IsValid())
    {
        return JsonValue.Value->AsNumber();
    }
    return 0.0f;
}

bool UAdvanceGameToolLibrary::Conv_JsonValueToBool(const FBlueprintJsonValue& JsonValue)
{
    if (JsonValue.Value.IsValid())
    {
        return JsonValue.Value->AsBool();
    }
    return false;
}

TArray<FBlueprintJsonValue> UAdvanceGameToolLibrary::Conv_JsonValueToArray(const FBlueprintJsonValue& JsonValue)
{
    TArray<FBlueprintJsonValue> Result;

    if (JsonValue.Value.IsValid())
    {
        if (JsonValue.Value->Type == EJson::Array)
        {
            for (const auto& Val : JsonValue.Value->AsArray())
            {
                FBlueprintJsonValue Tmp;
                Tmp.Value = Val;
                Result.Add(Tmp);
            }
        }
    }

    return Result;
}

FBlueprintJsonObject UAdvanceGameToolLibrary::Conv_JsonValueToObject(const FBlueprintJsonValue& JsonValue)
{
    FBlueprintJsonObject Object;
    if (JsonValue.Value.IsValid())
    {
        Object.Object = JsonValue.Value->AsObject();
    }
    return Object;
}

#pragma endregion