/** Copyright Mark Veligod. Published in 2023. **/

#include "AdvanceGameTools/Library/AdvanceGameToolLibrary.h"
#include "AdvanceGameTools/ThirdParty/AHEasing/AHEasing/easing.h"

#pragma region ActionMath

#pragma region MathEasing

float UAdvanceGameToolLibrary::EaseFloat(const ERyMathEasingType Easing, float Alpha)
{
    Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
    switch (Easing)
    {
        case ERyMathEasingType::Linear:
        {
            break;
        }
        case ERyMathEasingType::QuadraticEaseIn:
        {
            Alpha = AH::QuadraticEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::QuadraticEaseOut:
        {
            Alpha = AH::QuadraticEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::QuadraticEaseInOut:
        {
            Alpha = AH::QuadraticEaseInOut(Alpha);
            break;
        }
        case ERyMathEasingType::CubicEaseIn:
        {
            Alpha = AH::CubicEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::CubicEaseOut:
        {
            Alpha = AH::CubicEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::CubicEaseInOut:
        {
            Alpha = AH::CubicEaseInOut(Alpha);
            break;
        }
        case ERyMathEasingType::QuarticEaseIn:
        {
            Alpha = AH::QuarticEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::QuarticEaseOut:
        {
            Alpha = AH::QuarticEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::QuarticEaseInOut:
        {
            Alpha = AH::QuarticEaseInOut(Alpha);
            break;
        }
        case ERyMathEasingType::QuinticEaseIn:
        {
            Alpha = AH::QuinticEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::QuinticEaseOut:
        {
            Alpha = AH::QuinticEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::QuinticEaseInOut:
        {
            Alpha = AH::QuinticEaseInOut(Alpha);
            break;
        }
        case ERyMathEasingType::SineEaseIn:
        {
            Alpha = AH::SineEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::SineEaseOut:
        {
            Alpha = AH::SineEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::SineEaseInOut:
        {
            Alpha = AH::SineEaseInOut(Alpha);
            break;
        }
        case ERyMathEasingType::CircularEaseIn:
        {
            Alpha = AH::CircularEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::CircularEaseOut:
        {
            Alpha = AH::CircularEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::CircularEaseInOut:
        {
            Alpha = AH::CircularEaseInOut(Alpha);
            break;
        }
        case ERyMathEasingType::ExponentialEaseIn:
        {
            Alpha = AH::ExponentialEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::ExponentialEaseOut:
        {
            Alpha = AH::ExponentialEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::ExponentialEaseInOut:
        {
            Alpha = AH::ExponentialEaseInOut(Alpha);
            break;
        }
        case ERyMathEasingType::ElasticEaseIn:
        {
            Alpha = AH::ElasticEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::ElasticEaseOut:
        {
            Alpha = AH::ElasticEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::ElasticEaseInOut:
        {
            Alpha = AH::ElasticEaseInOut(Alpha);
            break;
        }
        case ERyMathEasingType::BackEaseIn:
        {
            Alpha = AH::BackEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::BackEaseOut:
        {
            Alpha = AH::BackEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::BackEaseInOut:
        {
            Alpha = AH::BackEaseInOut(Alpha);
            break;
        }
        case ERyMathEasingType::BounceEaseIn:
        {
            Alpha = AH::BounceEaseIn(Alpha);
            break;
        }
        case ERyMathEasingType::BounceEaseOut:
        {
            Alpha = AH::BounceEaseOut(Alpha);
            break;
        }
        case ERyMathEasingType::BounceEaseInOut:
        {
            Alpha = AH::BounceEaseInOut(Alpha);
            break;
        }
    }

    return Alpha;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 */
FVector UAdvanceGameToolLibrary::EaseVector(const ERyMathEasingType Easing, const FVector& Start, const FVector& Target, const float Alpha)
{
    if (Alpha == 0.0f)
    {
        return Start;
    }
    if (Alpha == 1.0f)
    {
        return Target;
    }
    return Start + UAdvanceGameToolLibrary::EaseFloat(Easing, Alpha) * (Target - Start);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 */
FRotator UAdvanceGameToolLibrary::EaseRotator(const ERyMathEasingType Easing, const FRotator& Start, const FRotator& Target, const float Alpha)
{
    if (Alpha == 0.0f)
    {
        return Start;
    }
    if (Alpha == 1.0f)
    {
        return Target;
    }

    const FRotator Delta = (Target - Start).GetNormalized();

    // If steps are too small, just return Target and assume we have reached our destination.
    if (Delta.IsNearlyZero())
    {
        return Target;
    }

    const FRotator DeltaMove = Delta * UAdvanceGameToolLibrary::EaseFloat(Easing, Alpha);
    return (Start + DeltaMove).GetNormalized();
}

#pragma endregion

#pragma region Angle

float UAdvanceGameToolLibrary::GetAngleInRadiansBetweenVectors(const FVector& VectorA, const FVector& VectorB)
{
    return FMath::Acos(FVector::DotProduct(VectorA, VectorB) / (VectorA.Size() * VectorB.Size()));
}

float UAdvanceGameToolLibrary::GetAngleBetweenVectors(const FVector& VectorA, const FVector& VectorB)
{
    return FMath::RadiansToDegrees(GetAngleInRadiansBetweenVectors(VectorA, VectorB));
}

float UAdvanceGameToolLibrary::GetAngleBetweenActors(AActor* A, AActor* B)
{
    if (!IsValid(A) || !IsValid(B)) return 0.0f;

    FVector DirectionToTarget = B->GetActorLocation() - A->GetActorLocation();
    DirectionToTarget.Z = A->GetActorForwardVector().Z;
    return GetAngleBetweenVectors(DirectionToTarget, A->GetActorForwardVector());
}

#pragma endregion

#pragma region Console

float UAdvanceGameToolLibrary::GetConsoleVariableFloat(const FString& VariableName)
{
    if (const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataFloat(*VariableName))
    {
        return CVar->GetValueOnGameThread();
    }

    return 0.f;
}

int32 UAdvanceGameToolLibrary::GetConsoleVariableInt(const FString& VariableName)
{
    if (const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(*VariableName))
    {
        return CVar->GetValueOnGameThread();
    }
    return 0;
}

bool UAdvanceGameToolLibrary::GetConsoleVariableBool(const FString& VariableName)
{
    if (const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataBool(*VariableName))
    {
        return CVar->GetValueOnGameThread();
    }
    return false;
}

#pragma endregion

#pragma region Numeric

int32 UAdvanceGameToolLibrary::RoundToLowerMultiple(int32 A, int32 Multiple, bool SkipSelf)
{
    const int32 result = (A / Multiple) * Multiple;
    if (SkipSelf && result == A && result != 0)
    {
        return ((A - 1) / Multiple) * Multiple;
    }
    return result;
}

int32 UAdvanceGameToolLibrary::RoundToUpperMultiple(int32 A, int32 Multiple, bool SkipSelf)
{
    if (SkipSelf || FMath::Fmod(static_cast<float>(A), static_cast<float>(Multiple)) != 0.0f)
    {
        A = ((A + Multiple) / Multiple) * Multiple;
    }
    return A;
}

int32 UAdvanceGameToolLibrary::RoundToNearestMultiple(int32 A, int32 Multiple)
{
    return ((A + Multiple / 2) / Multiple) * Multiple;
}

bool UAdvanceGameToolLibrary::IsPowerOfTwo(int32 x)
{
    return FMath::IsPowerOfTwo(x);
}

bool UAdvanceGameToolLibrary::IsMultipleOf(int32 A, int32 Multiple)
{
    return FMath::Fmod(static_cast<float>(A), static_cast<float>(Multiple)) == 0.0f;
}

bool UAdvanceGameToolLibrary::IsEvenNumber(float A)
{
    return FMath::Fmod(A, 2) == 0;
}

#pragma endregion

#pragma region Vector

FVector UAdvanceGameToolLibrary::ClosestPointOnSphereToLine(const FVector& SphereOrigin, float SphereRadius, const FVector& LineOrigin, const FVector& LineDir)
{
    FVector OutClosestPoint;
    FMath::SphereDistToLine(SphereOrigin, SphereRadius, LineOrigin, LineDir.GetSafeNormal(), OutClosestPoint);
    return OutClosestPoint;
}

FVector UAdvanceGameToolLibrary::ClosestPointOnLineSegment(const FVector& Point, const FVector& LineStart, const FVector& LineEnd)
{
    return FMath::ClosestPointOnLine(LineStart, LineEnd, Point);
}

FVector UAdvanceGameToolLibrary::ProjectPointOnLine(const FVector& LineOrigin, const FVector& LineDirection, const FVector& Point)
{
    const FVector SafeDir = LineDirection.GetSafeNormal();
    return LineOrigin + (SafeDir * ((Point - LineOrigin) | SafeDir));
}

void UAdvanceGameToolLibrary::ClosestPointsOfLineSegments(const FVector& Line1Start, const FVector& Line1End, const FVector& Line2Start, const FVector& Line2End, FVector& LinePoint1,
    FVector& LinePoint2)
{
    FMath::SegmentDistToSegmentSafe(Line1Start, Line1End, Line2Start, Line2End, LinePoint1, LinePoint2);
}

bool UAdvanceGameToolLibrary::ClosestPointsOnTwoLines(const FVector& LinePoint1, const FVector& LineVec1, const FVector& LinePoint2, const FVector& LineVec2, FVector& ClosestPointLine1,
    FVector& ClosestPointLine2)
{
    const float a = FVector::DotProduct(LineVec1, LineVec1);
    const float b = FVector::DotProduct(LineVec1, LineVec2);
    const float e = FVector::DotProduct(LineVec2, LineVec2);

    const float d = a * e - b * b;

    //lines are not parallel
    if (d != 0.0f)
    {
        const FVector r = LinePoint1 - LinePoint2;
        const float c = FVector::DotProduct(LineVec1, r);
        const float f = FVector::DotProduct(LineVec2, r);

        const float s = (b * f - c * e) / d;
        const float t = (a * f - c * b) / d;

        ClosestPointLine1 = LinePoint1 + LineVec1 * s;
        ClosestPointLine2 = LinePoint2 + LineVec2 * t;

        return true;
    }

    return false;
}

FVector UAdvanceGameToolLibrary::GridSnap(const FVector& A, float Grid)
{
    return A.GridSnap(Grid);
}

float UAdvanceGameToolLibrary::ConvertPhysicsLinearVelocity(const FVector& Velocity, ESpeedUnit SpeedUnit)
{
    if (Velocity.IsZero()) return 0.f;

    float unit = 0;
    switch (SpeedUnit)
    {
        case ESpeedUnit::CentimeterPerSecond:
        {
            unit = 1.0f;
            break;
        }
        case ESpeedUnit::FootPerSecond:
        {
            unit = 0.03280839895013f;
            break;
        }
        case ESpeedUnit::MeterPerSecond:
        {
            unit = 0.01f;
            break;
        }
        case ESpeedUnit::MeterPerMinute:
        {
            unit = 0.6f;
            break;
        }
        case ESpeedUnit::KilometerPerSecond:
        {
            unit = 0.00001f;
            break;
        }
        case ESpeedUnit::KilometerPerMinute:
        {
            unit = 0.0006f;
            break;
        }
        case ESpeedUnit::KilometerPerHour:
        {
            unit = 0.036f;
            break;
        }
        case ESpeedUnit::MilePerHour:
        {
            unit = 0.02236936292054f;
            break;
        }
        case ESpeedUnit::Knot:
        {
            unit = 0.01943844492441f;
            break;
        }
        case ESpeedUnit::Mach:
        {
            unit = 0.00002915451895044f;
            break;
        }
        case ESpeedUnit::SpeedOfLight:
        {
            unit = 3.335640951982E-11f;
            break;
        }
        case ESpeedUnit::YardPerSecond:
        {
            unit = 0.01093613298338f;
            break;
        }
        default: break;
    };

    return Velocity.Size() * unit;
}

bool UAdvanceGameToolLibrary::IsPointInsideBox(const FVector& Point, const FVector& BoxOrigin, const FVector& BoxExtent)
{
    const FBox Box = FBox::BuildAABB(BoxOrigin, BoxExtent);
    return FMath::PointBoxIntersection(Point, Box);
}

bool UAdvanceGameToolLibrary::IsLineInsideSphere(const FVector& LineStart, const FVector& LineDir, double LineLength, const FVector& SphereOrigin, double SphereRadius)
{
    return FMath::LineSphereIntersection(LineStart, LineDir, LineLength, SphereOrigin, SphereRadius);
}

#pragma endregion

#pragma region Physics

FVector UAdvanceGameToolLibrary::GetVelocityAtPoint(UPrimitiveComponent* Target, const FVector& Point, FName BoneName)
{
    if (!Target) return FVector::ZeroVector;

    const FBodyInstance* BI = Target->GetBodyInstance(BoneName);
    if (BI && BI->IsValidBodyInstance())
    {
        return BI->GetUnrealWorldVelocityAtPoint(Point);
    }

    return FVector::ZeroVector;
}

void UAdvanceGameToolLibrary::SetCenterOfMassOffset(UPrimitiveComponent* Target, const FVector& Offset, FName BoneName)
{
    if (!Target) return;

    FBodyInstance* BI = Target->GetBodyInstance(BoneName);
    if (BI && BI->IsValidBodyInstance())
    {
        BI->COMNudge = Offset;
        BI->UpdateMassProperties();
    }
}

#pragma endregion

#pragma region Random

FVector UAdvanceGameToolLibrary::RandomVectorInRange(const FVector& Start, const FVector& End)
{
    FVector Direction = End - Start;
    Direction.Normalize();

    return Start + Direction * FMath::RandRange(0.0f, static_cast<float>((End - Start).Size()));
}

FString UAdvanceGameToolLibrary::RandomStringFromAlphabet(const TArray<FString>& Alphabet, const int32 MinLettersCount, const int32 MaxLettersCount)
{
    FString GeneratedString = "";

    if (MaxLettersCount < MinLettersCount) return GeneratedString;

    const int RandomLettersCount = FMath::RandRange(MinLettersCount, MaxLettersCount);

    for (int i = 0; i < RandomLettersCount; ++i)
    {
        GeneratedString += Alphabet[FMath::RandRange(0, Alphabet.Num() - 1)];
    }

    return GeneratedString;
}

FColor UAdvanceGameToolLibrary::RandomColorInRange(const FColor& StartColor, const FColor& EndColor)
{
    return FColor(FMath::RandRange(StartColor.R, EndColor.R), FMath::RandRange(StartColor.G, EndColor.G), FMath::RandRange(StartColor.B, EndColor.B), FMath::RandRange(StartColor.A, EndColor.A));
}

void UAdvanceGameToolLibrary::RandomIntArray(int32 ArrayLength, int32 MinValue, int32 MaxValue, TArray<int32>& RandomArray)
{
    if (ArrayLength <= 0 || MaxValue < MinValue) return;

    RandomArray.Empty();
    for (int i = 0; i < ArrayLength; ++i)
    {
        RandomArray.Add(FMath::RandRange(MinValue, MaxValue));
    }
}

#pragma endregion

#pragma endregion
