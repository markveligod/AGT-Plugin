/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "Kismet/KismetStringLibrary.h"

#pragma region AdvanceArray

#pragma region Average

template <typename T>
float UAdvanceGameToolLibrary::Average(const TArray<T>& Array)
{
    float Average = 0.0f;
    int32 Total = Array.Num();
    Total = Total > 0 ? Total : 1;

    for (const T& Value : Array)
    {
        Average += Value;
    }

    return Average / Total;
}

float UAdvanceGameToolLibrary::AverageInteger(const TArray<int32>& Array)
{
    return UAdvanceGameToolLibrary::Average(Array);
}

float UAdvanceGameToolLibrary::AverageFloat(const TArray<float>& Array)
{
    return UAdvanceGameToolLibrary::Average(Array);
}

#pragma endregion

#pragma region Minimum

template <typename T>
int32 UAdvanceGameToolLibrary::Minimum(const TArray<T>& Array)
{
    int32 Index = Array.Num() > 0 ? 0 : -1;
    for (int32 i = 1; i < Array.Num(); i++)
    {
        if (Array[i] < Array[Index])
        {
            Index = i;
        }
    }
    return Index;
}

int32 UAdvanceGameToolLibrary::MinimumIntegerIndex(const TArray<int32>& Array)
{
    return UAdvanceGameToolLibrary::Minimum(Array);
}

int32 UAdvanceGameToolLibrary::MinimumFloatIndex(const TArray<float>& Array)
{
    return UAdvanceGameToolLibrary::Minimum(Array);
}

#pragma endregion

#pragma region Maximum

template <typename T>
int32 UAdvanceGameToolLibrary::Maximum(const TArray<T>& Array)
{
    int32 Index = Array.Num() > 0 ? 0 : -1;
    for (int32 i = 1; i < Array.Num(); i++)
    {
        if (Array[i] > Array[Index])
        {
            Index = i;
        }
    }
    return Index;
}

int32 UAdvanceGameToolLibrary::MaximumIntegerIndex(const TArray<int32>& Array)
{
    return UAdvanceGameToolLibrary::Maximum(Array);
}

int32 UAdvanceGameToolLibrary::MaximumFloatIndex(const TArray<float>& Array)
{
    return UAdvanceGameToolLibrary::Maximum(Array);
}

#pragma endregion

#pragma region Normalize

template <typename T>
TArray<T> UAdvanceGameToolLibrary::MinMaxNormalization(const TArray<T>& Array, T Min, T Max)
{
    TArray<T> NormalizedArray;
    if (Array.Num() > 0)
    {
        NormalizedArray.SetNumUninitialized(Array.Num());
        const int32 MinIdx = UAdvanceGameToolLibrary::Minimum(Array);
        const int32 MaxIdx = UAdvanceGameToolLibrary::Maximum(Array);
        const T MinVal = Array[MinIdx];
        const T MaxVal = Array[MaxIdx] != 0 ? Array[MaxIdx] : 1;
        for (int32 i = 0; i < Array.Num(); i++)
        {
            NormalizedArray[i] = (((Array[i] - MinVal) / (MaxVal * 1.0f)) * (Max - Min)) + Min;
        }
    }
    return NormalizedArray;
}

TArray<float> UAdvanceGameToolLibrary::MinMaxFloatNormalization(const TArray<float>& Array, float Min, float Max)
{
    return UAdvanceGameToolLibrary::MinMaxNormalization(Array, Min, Max);
}

TArray<int32> UAdvanceGameToolLibrary::MinMaxIntegerNormalization(const TArray<int32>& Array, int32 Min, int32 Max)
{
    return UAdvanceGameToolLibrary::MinMaxNormalization(Array, Min, Max);
}

#pragma endregion

#pragma region Reverse

template <typename T>
TArray<T> UAdvanceGameToolLibrary::Reverse(const TArray<T>& Array)
{
    TArray<T> ReversedArray;
    for (auto i = Array.Num() - 1; i >= 0; --i)
    {
        ReversedArray.Add(Array[i]);
    }
    return ReversedArray;
}

TArray<int32> UAdvanceGameToolLibrary::ReverseInteger(const TArray<int32>& Array)
{
    return UAdvanceGameToolLibrary::Reverse(Array);
}

TArray<float> UAdvanceGameToolLibrary::ReverseFloat(const TArray<float>& Array)
{
    return UAdvanceGameToolLibrary::Reverse(Array);
}

TArray<FString> UAdvanceGameToolLibrary::ReverseString(const TArray<FString>& Array)
{
    return UAdvanceGameToolLibrary::Reverse(Array);
}

TArray<FName> UAdvanceGameToolLibrary::ReverseName(const TArray<FName>& Array)
{
    return UAdvanceGameToolLibrary::Reverse(Array);
}

TArray<FVector> UAdvanceGameToolLibrary::ReverseVector(const TArray<FVector>& Array)
{
    return UAdvanceGameToolLibrary::Reverse(Array);
}

TArray<AActor*> UAdvanceGameToolLibrary::ReverseActor(const TArray<AActor*>& Array)
{
    return UAdvanceGameToolLibrary::Reverse(Array);
}

TArray<UObject*> UAdvanceGameToolLibrary::ReverseObject(const TArray<UObject*>& Array)
{
    return UAdvanceGameToolLibrary::Reverse(Array);
}

#pragma endregion

#pragma region Convert

TArray<FString> UAdvanceGameToolLibrary::SplitString(FString String, const FString& Separator, ESearchCase::Type SearchCase, bool RemoveEmptyString)
{
    FString LeftString;
    FString RightString;
    TArray<FString> Array;
    bool Split = false;
    do
    {
        Split = String.Split(Separator, &LeftString, &RightString, SearchCase);
        if (Split)
        {
            if (RemoveEmptyString)
            {
                if (!LeftString.IsEmpty())
                {
                    Array.Add(LeftString);
                }
            }
            else
            {
                Array.Add(LeftString);
            }
        }
        else
        {
            if (RemoveEmptyString)
            {
                if (!String.IsEmpty())
                {
                    Array.Add(String);
                }
            }
            else
            {
                Array.Add(String);
            }
        }
        String = RightString;
    } while (Split);

    return Array;
}

FString UAdvanceGameToolLibrary::SplitStringBySizeToOneLine(FString String, int32 Size)
{
    FString Result;

    while (!String.IsEmpty())
    {
        Result.Append(UKismetStringLibrary::GetSubstring(String, 0, Size) + "\n");
        String.RemoveAt(0, FMath::Clamp(Size, 0, String.Len()));
    }

    return Result;
}

TArray<FString> UAdvanceGameToolLibrary::SplitStringBySizeToArray(FString String, int32 Size)
{
    TArray<FString> Result;

    while (!String.IsEmpty())
    {
        Result.Add(UKismetStringLibrary::GetSubstring(String, 0, Size));
        String.RemoveAt(0, FMath::Clamp(Size, 0, String.Len()));
    }

    return Result;
}

#pragma endregion

#pragma region Clamp

template <typename T>
TArray<T> UAdvanceGameToolLibrary::Clamp(TArray<T>& Array, T Min, T Max)
{
    if (Min < Max)
    {
        for (int i = 0; i < Array.Num(); i++)
        {
            if (Array[i] < Min)
            {
                Array[i] = Min;
            }
            else if (Array[i] > Max)
            {
                Array[i] = Max;
            }
        }
    }
    return Array;
}

TArray<int32> UAdvanceGameToolLibrary::ClampInteger(TArray<int32> Array, int32 Min, int32 Max)
{
    return UAdvanceGameToolLibrary::Clamp(Array, Min, Max);
}

TArray<float> UAdvanceGameToolLibrary::ClampFloat(TArray<float> Array, float Min, float Max)
{
    return UAdvanceGameToolLibrary::Clamp(Array, Min, Max);
}

TArray<FVector> UAdvanceGameToolLibrary::ClampVectorSize(TArray<FVector> Array, float MinSize, float MaxSize, bool bOnly2D)
{
    if (MinSize < MaxSize)
    {
        for (int i = 0; i < Array.Num(); i++)
        {
            float Size = bOnly2D ? Array[i].Size2D() : Array[i].Size();
            if (Size < MinSize || Size > MaxSize)
            {
                Array[i] = bOnly2D ? Array[i].GetClampedToSize2D(MinSize, MaxSize) : Array[i].GetClampedToSize(MinSize, MaxSize);
            }
        }
    }
    return Array;
}

void UAdvanceGameToolLibrary::ClampIntegerRef(TArray<int32>& Array, int32 Min, int32 Max)
{
    UAdvanceGameToolLibrary::Clamp(Array, Min, Max);
}

void UAdvanceGameToolLibrary::ClampFloatRef(TArray<float>& Array, float Min, float Max)
{
    UAdvanceGameToolLibrary::Clamp(Array, Min, Max);
}

void UAdvanceGameToolLibrary::ClampVectorSizeRef(TArray<FVector>& Array, float MinSize, float MaxSize, bool bOnly2D)
{
    if (MinSize < MaxSize)
    {
        for (int i = 0; i < Array.Num(); i++)
        {
            float Size = bOnly2D ? Array[i].Size2D() : Array[i].Size();
            if (Size < MinSize || Size > MaxSize)
            {
                Array[i] = bOnly2D ? Array[i].GetClampedToSize2D(MinSize, MaxSize) : Array[i].GetClampedToSize(MinSize, MaxSize);
            }
        }
    }
}

#pragma endregion

#pragma region Range

template <typename T>
TArray<T> UAdvanceGameToolLibrary::Extract(const TArray<T>& Array, int32 StartIndex, int32 EndIndex)
{
    TArray<T> SubArray;
    if (StartIndex < EndIndex && EndIndex < Array.Num())
    {
        for (int i = StartIndex; i <= EndIndex; i++)
        {
            SubArray.Add(Array[i]);
        }
    }
    return SubArray;
}

TArray<int32> UAdvanceGameToolLibrary::ExtractInteger(const TArray<int32>& Array, int32 StartIndex, int32 EndIndex)
{
    return UAdvanceGameToolLibrary::Extract(Array, StartIndex, EndIndex);
}

TArray<float> UAdvanceGameToolLibrary::ExtractFloat(const TArray<float>& Array, int32 StartIndex, int32 EndIndex)
{
    return UAdvanceGameToolLibrary::Extract(Array, StartIndex, EndIndex);
}

TArray<FString> UAdvanceGameToolLibrary::ExtractString(const TArray<FString>& Array, int32 StartIndex, int32 EndIndex)
{
    return UAdvanceGameToolLibrary::Extract(Array, StartIndex, EndIndex);
}

TArray<FName> UAdvanceGameToolLibrary::ExtractName(const TArray<FName>& Array, int32 StartIndex, int32 EndIndex)
{
    return UAdvanceGameToolLibrary::Extract(Array, StartIndex, EndIndex);
}

TArray<FVector> UAdvanceGameToolLibrary::ExtractVector(const TArray<FVector>& Array, int32 StartIndex, int32 EndIndex)
{
    return UAdvanceGameToolLibrary::Extract(Array, StartIndex, EndIndex);
}

TArray<AActor*> UAdvanceGameToolLibrary::ExtractActor(const TArray<AActor*>& Array, int32 StartIndex, int32 EndIndex)
{
    return UAdvanceGameToolLibrary::Extract(Array, StartIndex, EndIndex);
}

TArray<UObject*> UAdvanceGameToolLibrary::ExtractObject(const TArray<UObject*>& Array, int32 StartIndex, int32 EndIndex)
{
    return UAdvanceGameToolLibrary::Extract(Array, StartIndex, EndIndex);
}

#pragma endregion

#pragma region Random

template <typename T>
TArray<T> UAdvanceGameToolLibrary::Random(int32 Size, T& MinValue, T& MaxValue)
{
    TArray<T> Array;
    if (Size > 0 && MinValue < MaxValue)
    {
        for (int i = 0; i < Size; i++)
        {
            Array.Add(FMath::RandRange(MinValue, MaxValue));
        }
    }
    return Array;
}

TArray<int32> UAdvanceGameToolLibrary::RandomInteger(int32 Size, int32 MinValue, int32 MaxValue)
{
    return UAdvanceGameToolLibrary::Random(Size, MinValue, MaxValue);
}

TArray<float> UAdvanceGameToolLibrary::RandomFloat(int32 Size, float MinValue, float MaxValue)
{
    return UAdvanceGameToolLibrary::Random(Size, MinValue, MaxValue);
}

TArray<FVector> UAdvanceGameToolLibrary::RandomVector(int32 Size, FVector MinValue, FVector MaxValue)
{
    TArray<FVector> Array;
    if (Size > 0 && MinValue.X <= MaxValue.X && MinValue.Y <= MaxValue.Y && MinValue.Z <= MaxValue.Z)
    {
        for (int i = 0; i < Size; i++)
        {
            Array.Add(FVector(FMath::FRandRange(MinValue.X, MaxValue.X), FMath::FRandRange(MinValue.Y, MaxValue.Y), FMath::FRandRange(MinValue.Z, MaxValue.Z)));
        }
    }
    return Array;
}

#pragma endregion

#pragma region Sorts

template <typename T>
TArray<T> UAdvanceGameToolLibrary::Sort(TArray<T>& Array, bool bIsAscending)
{
    if (bIsAscending)
    {
        Array.Sort();
    }
    else
    {
        Array.Sort(TReverseSortPredicate<T>());
    }
    return Array;
}

TArray<int32> UAdvanceGameToolLibrary::SortInteger(TArray<int32> Array, bool bIsAscending)
{
    return UAdvanceGameToolLibrary::Sort(Array, bIsAscending);
}

TArray<float> UAdvanceGameToolLibrary::SortFloat(TArray<float> Array, bool bIsAscending)
{
    return UAdvanceGameToolLibrary::Sort(Array, bIsAscending);
}

TArray<FString> UAdvanceGameToolLibrary::SortString(TArray<FString> Array, bool bIsAscending)
{
    return UAdvanceGameToolLibrary::Sort(Array, bIsAscending);
}

TArray<FName> UAdvanceGameToolLibrary::SortName(TArray<FName> Array, bool bIsAscending)
{
    // DO NOT USE FOR FNAME -> return Sort<FName>(Array, bIsAscending)
    if (bIsAscending)
    {
        Array.Sort([](const FName A, const FName B) { return A.LexicalLess(B); });
    }
    else
    {
        Array.Sort([](const FName A, const FName B) { return B.LexicalLess(A); });
    }
    return Array;
}

TArray<AActor*> UAdvanceGameToolLibrary::SortActor(TArray<AActor*> Array, AActor* const& Actor, bool bIsAscending)
{
    if (Actor != nullptr)
    {
        if (bIsAscending)
        {
            Array.Sort([Actor](AActor& A, AActor& B) { return (&A)->GetDistanceTo(Actor) < (&B)->GetDistanceTo(Actor); });
        }
        else
        {
            Array.Sort([Actor](AActor& A, AActor& B) { return (&A)->GetDistanceTo(Actor) > (&B)->GetDistanceTo(Actor); });
        }
    }
    return Array;
}

TArray<FVector> UAdvanceGameToolLibrary::SortVector(TArray<FVector> Array, FVector Origin, bool bIsAscending)
{
    if (bIsAscending)
    {
        Array.Sort([Origin](FVector A, FVector B) { return FVector::Distance(A, Origin) < FVector::Distance(B, Origin); });
    }
    else
    {
        Array.Sort([Origin](FVector A, FVector B) { return FVector::Distance(A, Origin) > FVector::Distance(B, Origin); });
    }
    return Array;
}

void UAdvanceGameToolLibrary::SortIntegerRef(TArray<int32>& Array, bool bIsAscending)
{
    UAdvanceGameToolLibrary::Sort(Array, bIsAscending);
}

void UAdvanceGameToolLibrary::SortFloatRef(TArray<float>& Array, bool bIsAscending)
{
    UAdvanceGameToolLibrary::Sort(Array, bIsAscending);
}

void UAdvanceGameToolLibrary::SortStringRef(TArray<FString>& Array, bool bIsAscending)
{
    UAdvanceGameToolLibrary::Sort(Array, bIsAscending);
}

void UAdvanceGameToolLibrary::SortNameRef(TArray<FName>& Array, bool bIsAscending)
{
    // DO NOT USE FOR FNAME -> Sort<FName>(Array, bIsAscending);
    if (bIsAscending)
    {
        Array.Sort([](FName A, FName B) { return A.LexicalLess(B); });
    }
    else
    {
        Array.Sort([](FName A, FName B) { return B.LexicalLess(A); });
    }
}

void UAdvanceGameToolLibrary::SortActorRef(TArray<AActor*>& Array, AActor* const& Actor, bool bIsAscending)
{
    if (Actor != nullptr)
    {
        if (bIsAscending)
        {
            Array.Sort([Actor](AActor& A, AActor& B) { return (&A)->GetDistanceTo(Actor) < (&B)->GetDistanceTo(Actor); });
        }
        else
        {
            Array.Sort([Actor](AActor& A, AActor& B) { return (&A)->GetDistanceTo(Actor) > (&B)->GetDistanceTo(Actor); });
        }
    }
}

void UAdvanceGameToolLibrary::SortVectorRef(TArray<FVector>& Array, FVector Origin, bool bIsAscending)
{
    if (bIsAscending)
    {
        Array.Sort([Origin](FVector A, FVector B) { return FVector::Distance(A, Origin) < FVector::Distance(B, Origin); });
    }
    else
    {
        Array.Sort([Origin](FVector A, FVector B) { return FVector::Distance(A, Origin) > FVector::Distance(B, Origin); });
    }
}

#pragma endregion

#pragma region SortPredicate

TArray<int32> UAdvanceGameToolLibrary::PredicateSortInteger(TArray<int32> Array, const FIntSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const int32& A, const int32& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
    return Array;
}

TArray<float> UAdvanceGameToolLibrary::PredicateSortFloat(TArray<float> Array, const FFloatSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const float& A, const float& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
    return Array;
}

TArray<FString> UAdvanceGameToolLibrary::PredicateSortString(TArray<FString> Array, const FStringSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const FString& A, const FString& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
    return Array;
}

TArray<FName> UAdvanceGameToolLibrary::PredicateSortName(TArray<FName> Array, const FNameSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const FName& A, const FName& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
    return Array;
}

TArray<FVector> UAdvanceGameToolLibrary::PredicateSortVector(TArray<FVector> Array, const FVectorSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const FVector& A, const FVector& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
    return Array;
}

TArray<AActor*> UAdvanceGameToolLibrary::PredicateSortActor(TArray<AActor*> Array, const FActorSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](AActor& A, AActor& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(&A, &B, Result);
            return InvertResult ? !Result : Result;
        });
    return Array;
}

TArray<UObject*> UAdvanceGameToolLibrary::PredicateSortObject(TArray<UObject*> Array, const FObjectSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](UObject& A, UObject& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(&A, &B, Result);
            return InvertResult ? !Result : Result;
        });
    return Array;
}

void UAdvanceGameToolLibrary::PredicateSortIntegerRef(TArray<int32>& Array, const FIntSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const int32& A, const int32& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
}

void UAdvanceGameToolLibrary::PredicateSortFloatRef(TArray<float>& Array, const FFloatSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const float& A, const float& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
}

void UAdvanceGameToolLibrary::PredicateSortStringRef(TArray<FString>& Array, const FStringSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const FString& A, const FString& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
}

void UAdvanceGameToolLibrary::PredicateSortNameRef(TArray<FName>& Array, const FNameSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const FName& A, const FName& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
}

void UAdvanceGameToolLibrary::PredicateSortVectorRef(TArray<FVector>& Array, const FVectorSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](const FVector& A, const FVector& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, B, Result);
            return InvertResult ? !Result : Result;
        });
}

void UAdvanceGameToolLibrary::PredicateSortActorRef(TArray<AActor*>& Array, const FActorSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](AActor& A, AActor& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(&A, &B, Result);
            return InvertResult ? !Result : Result;
        });
}

void UAdvanceGameToolLibrary::PredicateSortObjectRef(TArray<UObject*>& Array, const FObjectSortDelegate& PredicateFunction, bool InvertResult)
{
    Array.Sort(
        [PredicateFunction, InvertResult](UObject& A, UObject& B)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(&A, &B, Result);
            return InvertResult ? !Result : Result;
        });
}

#pragma endregion

#pragma region Distance

void UAdvanceGameToolLibrary::ClosestLocation(const TArray<FVector>& Array, FVector Origin, FVector& Closest, float& Distance, int32& Index)
{
    float ClosestDistance = 0;
    Index = -1;
    for (int i = 0; i < Array.Num(); i++)
    {
        FVector Vector = Array[i];
        Distance = FVector::Distance(Origin, Vector);
        if (i == 0 || Distance < ClosestDistance)
        {
            Index = i;
            ClosestDistance = Distance;
            Closest = Vector;
        }
    }
    Distance = ClosestDistance;
}

void UAdvanceGameToolLibrary::FarthestLocation(const TArray<FVector>& Array, FVector Origin, FVector& Farthest, float& Distance, int32& Index)
{
    float FarthestDistance = 0;
    Index = -1;
    for (int i = 0; i < Array.Num(); i++)
    {
        FVector Vector = Array[i];
        Distance = FVector::Distance(Origin, Vector);
        if (i == 0 || Distance > FarthestDistance)
        {
            Index = i;
            FarthestDistance = Distance;
            Farthest = Vector;
        }
    }
    Distance = FarthestDistance;
}

void UAdvanceGameToolLibrary::ClosestActor(const TArray<AActor*>& Array, AActor* const& Origin, AActor*& Closest, float& Distance, int32& Index)
{
    float ClosestDistance = 0;
    Index = -1;
    if (Origin == nullptr)
    {
        return;
    }
    for (int i = 0; i < Array.Num(); i++)
    {
        AActor* Actor = Array[i];
        Distance = Origin->GetDistanceTo(Actor);
        if (i == 0 || Distance < ClosestDistance)
        {
            Index = i;
            ClosestDistance = Distance;
            Closest = Actor;
        }
    }
    Distance = ClosestDistance;
}

void UAdvanceGameToolLibrary::FarthestActor(const TArray<AActor*>& Array, AActor* const& Origin, AActor*& Farthest, float& Distance, int32& Index)
{
    float FarthestDistance = 0;
    Index = -1;
    if (Origin == nullptr)
    {
        return;
    }
    for (int i = 0; i < Array.Num(); i++)
    {
        AActor* Actor = Array[i];
        Distance = Origin->GetDistanceTo(Actor);
        if (i == 0 || Distance > FarthestDistance)
        {
            Index = i;
            FarthestDistance = Distance;
            Farthest = Actor;
        }
    }
    Distance = FarthestDistance;
}

#pragma endregion

#pragma region Filters

TArray<FString> UAdvanceGameToolLibrary::FilterMatches(const TArray<FString>& Array, const FString& Pattern, bool& bFound, TArray<int32>& Indexes)
{
    const FRegexPattern customPattern(Pattern);
    TArray<FString> Result;
    bFound = false;
    for (int32 i = 0; i < Array.Num(); i++)
    {
        FRegexMatcher CustomMatcher(customPattern, Array[i]);

        if (CustomMatcher.FindNext())
        {
            bFound = true;
            Result.Add(Array[i]);
            Indexes.Add(i);
        }
    }
    return Result;
}

FString UAdvanceGameToolLibrary::FilterMatch(const TArray<FString>& Array, const FString& Pattern, bool& bFound, int32& Index)
{
    const FRegexPattern customPattern(Pattern);
    bFound = false;
    Index = 0;
    for (FString String : Array)
    {
        FRegexMatcher CustomMatcher(customPattern, String);
        if (CustomMatcher.FindNext())
        {
            bFound = true;
            return String;
        }
        Index++;
    }
    Index = -1;
    return FString();
}

#pragma endregion

#pragma region FiltersPredicate

TArray<int32> UAdvanceGameToolLibrary::PredicateFilterInteger(const TArray<int32>& Array, const FIntFilterDelegate& PredicateFunction, bool InvertResult)
{
    return Array.FilterByPredicate(
        [PredicateFunction, InvertResult](const int32& A)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, Result);
            return InvertResult ? !Result : Result;
        });
}

TArray<float> UAdvanceGameToolLibrary::PredicateFilterFloat(const TArray<float>& Array, const FFloatFilterDelegate& PredicateFunction, bool InvertResult)
{
    return Array.FilterByPredicate(
        [PredicateFunction, InvertResult](const float& A)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, Result);
            return InvertResult ? !Result : Result;
        });
}

TArray<FString> UAdvanceGameToolLibrary::PredicateFilterString(const TArray<FString>& Array, const FStringFilterDelegate& PredicateFunction, bool InvertResult)
{
    return Array.FilterByPredicate(
        [PredicateFunction, InvertResult](const FString& A)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, Result);
            return InvertResult ? !Result : Result;
        });
}

TArray<FName> UAdvanceGameToolLibrary::PredicateFilterName(const TArray<FName>& Array, const FNameFilterDelegate& PredicateFunction, bool InvertResult)
{
    return Array.FilterByPredicate(
        [PredicateFunction, InvertResult](const FName& A)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, Result);
            return InvertResult ? !Result : Result;
        });
}

TArray<FVector> UAdvanceGameToolLibrary::PredicateFilterVector(const TArray<FVector>& Array, const FVectorFilterDelegate& PredicateFunction, bool InvertResult)
{
    return Array.FilterByPredicate(
        [PredicateFunction, InvertResult](const FVector& A)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, Result);
            return InvertResult ? !Result : Result;
        });
}

TArray<AActor*> UAdvanceGameToolLibrary::PredicateFilterActor(const TArray<AActor*>& Array, const FActorFilterDelegate& PredicateFunction, bool InvertResult)
{
    return Array.FilterByPredicate(
        [PredicateFunction, InvertResult](const AActor* A)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, Result);
            return InvertResult ? !Result : Result;
        });
}

TArray<UObject*> UAdvanceGameToolLibrary::PredicateFilterObject(const TArray<UObject*>& Array, const FObjectFilterDelegate& PredicateFunction, bool InvertResult)
{
    return Array.FilterByPredicate(
        [PredicateFunction, InvertResult](const UObject* A)
        {
            bool Result;
            PredicateFunction.ExecuteIfBound(A, Result);
            return InvertResult ? !Result : Result;
        });
}

#pragma endregion

#pragma endregion

