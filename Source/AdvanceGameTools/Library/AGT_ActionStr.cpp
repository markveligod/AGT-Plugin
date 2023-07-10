/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "Misc/Paths.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTableRegistry.h"

#pragma region ActionStr

bool UAdvanceGameToolLibrary::IsNone(const FName Name)
{
    return Name.IsNone();
}

bool UAdvanceGameToolLibrary::IsEmpty(const FString& String)
{
    return String.IsEmpty();
}

void UAdvanceGameToolLibrary::SetChar(FString& SourceString, const int32 CharIndex, const FString& CharIn)
{
    if (SourceString.Len() <= CharIndex || CharIn.Len() == 0 || CharIndex < 0)
    {
        return;
    }

    TArray<TCHAR>& chars = SourceString.GetCharArray();
    chars[CharIndex] = **CharIn;
}

FString UAdvanceGameToolLibrary::GetChar(FString& SourceString, const int32 CharIndex)
{
    if (SourceString.Len() <= CharIndex || CharIndex < 0)
    {
        return TEXT("");
    }

    FString CharOut;
    TArray<TCHAR>& Chars = SourceString.GetCharArray();
    CharOut += Chars[CharIndex];
    return CharOut;
}

void UAdvanceGameToolLibrary::PopChar(FString& SourceString)
{
    if (!SourceString.Len())
    {
        return;
    }
    SourceString.RemoveAt(SourceString.Len() - 1, 1, false);
}

void UAdvanceGameToolLibrary::PushChar(FString& SourceString, const FString& CharIn)
{
    if (!CharIn.Len())
    {
        return;
    }

    SourceString.InsertAt(SourceString.Len(), **CharIn);
}

void UAdvanceGameToolLibrary::FillString(FString& SourceString, const int32 NumChars, const FString& CharIn)
{
    if (CharIn.Len() == 0 || NumChars <= 0)
    {
        return;
    }

    const TCHAR c = **CharIn;
    TArray<TCHAR>& chars = SourceString.GetCharArray();
    chars.SetNumUninitialized(NumChars + 1);
    for (int32 cIndex = 0; cIndex < NumChars; ++cIndex)
    {
        chars[cIndex] = c;
    }
    chars[NumChars] = 0;
}

void UAdvanceGameToolLibrary::SetReserved(FString& SourceString, const int32 ReserveNumChars)
{
    if (ReserveNumChars > 0)
    {
        SourceString.Reserve(ReserveNumChars);
    }
}

void UAdvanceGameToolLibrary::AppendTo(FString& SourceString, const FString& Appendage)
{
    SourceString.Reserve(SourceString.Len() + Appendage.Len());
    SourceString += Appendage;
}

void UAdvanceGameToolLibrary::AppendStringsTo(FString& SourceString, const TArray<FString>& Appendages)
{
    int32 NumNewChars = 0;
    for (int32 AppendageIndex = 0; AppendageIndex < Appendages.Num(); ++AppendageIndex)
    {
        NumNewChars += Appendages[AppendageIndex].Len();
    }

    SourceString.Reserve(SourceString.Len() + NumNewChars);

    for (int32 AppendageIndex = 0; AppendageIndex < Appendages.Num(); ++AppendageIndex)
    {
        SourceString += Appendages[AppendageIndex];
    }
}

void UAdvanceGameToolLibrary::CharToBytes(const FString& sourceString, const int32 charIndex, uint8& byte1, uint8& byte2)
{
    if (sourceString.Len() <= charIndex || charIndex < 0)
    {
        byte1 = 0;
        byte2 = 0;
        return;
    }

    const TArray<TCHAR>& chars = sourceString.GetCharArray();
    constexpr int32 cSize = sizeof(TCHAR);

    byte1 = 0;
    byte2 = ((uint8*)(&chars[charIndex]))[0];
    if constexpr (cSize > 1)
    {
        byte1 = ((uint8*)(&chars[charIndex]))[0];
        byte2 = ((uint8*)(&chars[charIndex]))[1];
    }
}

void UAdvanceGameToolLibrary::ToTitleStringInline(FString& InOutString)
{
    bool bMakeUpper = true;
    const int32 StringLength = InOutString.Len();
    TArray<TCHAR>& RawData = InOutString.GetCharArray();
    for (int32 i = 0; i < StringLength; ++i)
    {
        if (FChar::IsWhitespace(RawData[i]))
        {
            bMakeUpper = true;
            continue;
        }

        if (bMakeUpper)
        {
            RawData[i] = FChar::ToUpper(RawData[i]);
            bMakeUpper = false;
        }
        else
        {
            RawData[i] = FChar::ToLower(RawData[i]);
        }
    }
}

FString UAdvanceGameToolLibrary::ToTitleString(const FString& InString)
{
    FString TitleStrOut = InString;
    ToTitleStringInline(TitleStrOut);
    return TitleStrOut;
}

FText UAdvanceGameToolLibrary::GetTextFromStringTable(UStringTable* Table, const FString& Key, bool& bIsValid)
{
    bIsValid = false;
    if (!Table)
    {
        return FText::GetEmpty();
    }

    return GetTextFromStringTableID(Table->GetStringTableId(), Key, bIsValid);
}

FText UAdvanceGameToolLibrary::GetTextFromStringTableID(FName TableID, const FString& Key, bool& bIsValid)
{
    FStringTableConstPtr StringTable = FStringTableRegistry::Get().FindStringTable(TableID);
    if (!StringTable.IsValid())
    {
        // Try to load it
        IStringTableEngineBridge::FullyLoadStringTableAsset(TableID);
        StringTable = FStringTableRegistry::Get().FindStringTable(TableID);
    }
    if (StringTable.IsValid())
    {
        const FStringTableEntryConstPtr strTableEntry = StringTable->FindEntry(Key);
        if (strTableEntry.IsValid())
        {
            bIsValid = true;
            return FText::FromStringTable(TableID, Key);
        }
    }

    return FText::GetEmpty();
}

FString UAdvanceGameToolLibrary::Conv_Int64ToString(const int64 InInt64)
{
    return FString::Printf(TEXT("%lld"), InInt64);
}

FString UAdvanceGameToolLibrary::CombineFilePaths(FString A, FString B)
{
    return FPaths::Combine(A, B);
}

#pragma endregion
