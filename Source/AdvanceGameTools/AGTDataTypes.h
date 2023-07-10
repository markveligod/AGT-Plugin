#pragma once

#include "CoreMinimal.h"
#include "AGTDataTypes.generated.h"

/** Sort **/
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FIntSortDelegate, const int32&, A, const int32&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FFloatSortDelegate, const float&, A, const float&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FStringSortDelegate, const FString&, A, const FString&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FNameSortDelegate, const FName&, A, const FName&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FVectorSortDelegate, const FVector&, A, const FVector&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FActorSortDelegate, const AActor*, A, const AActor*, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FObjectSortDelegate, const UObject*, A, const UObject*, B, bool&, Result);

/** Filter **/
DECLARE_DYNAMIC_DELEGATE_TwoParams(FIntFilterDelegate, const int32&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FFloatFilterDelegate, const float&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FStringFilterDelegate, const FString&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FNameFilterDelegate, const FName&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FVectorFilterDelegate, const FVector&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FActorFilterDelegate, const AActor*, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FObjectFilterDelegate, const UObject*, Value, bool&, Result);

/** Spawn **/
DECLARE_DYNAMIC_DELEGATE_TwoParams(FAsyncAssetLoadSignature, bool, bResult, UObject*, Object);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FAsyncSpawnActorSignature, bool, bResult, AActor*, SpawnedActor);

USTRUCT(BlueprintType)
struct FAdvanceActorParameters
{
    GENERATED_BODY()

    /* A name to assign as the Name of the Actor being spawned. If no value is specified, the name of the spawned Actor will be automatically generated using the form [Class]_[Number]. */
    UPROPERTY(BlueprintReadWrite)
    FName Name{NAME_None};

    /* An Actor to use as a template when spawning the new Actor. The spawned Actor will be initialized using the property values of the template Actor. If left NULL the class default object (CDO)
     * will be used to initialize the spawned Actor. */
    UPROPERTY(BlueprintReadWrite)
    AActor* Template{nullptr};

    /* The Actor that spawned this Actor. (Can be left as NULL). */
    UPROPERTY(BlueprintReadWrite)
    AActor* Owner{nullptr};

    /* The APawn that is responsible for damage done by the spawned Actor. (Can be left as NULL). */
    UPROPERTY(BlueprintReadWrite)
    APawn* Instigator{nullptr};

    /** Method for resolving collisions at the spawn point. Undefined means no override, use the actor's setting. */
    UPROPERTY(BlueprintReadWrite)
    ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride{ESpawnActorCollisionHandlingMethod::AlwaysSpawn};

    /** Parameters for the actor spawn */
    UPROPERTY(BlueprintReadWrite)
    FVector Location;

    /** Parameters for the actor spawn */
    UPROPERTY(BlueprintReadWrite)
    FRotator Rotation;
};

template <typename T>
class TReverseSortPredicate
{
public:
    bool operator()(const T A, const T B) const { return A > B; }
};

UENUM(BlueprintType)
enum class EPlatformType : uint8
{
    None UMETA(Hidden),
    Widows UMETA(DisplayName = "Widows"),
    Linux UMETA(DisplayName = "Linux"),
    Mac UMETA(DisplayName = "Mac"),
    Hololens UMETA(DisplayName = "Hololens"),
    IOS UMETA(DisplayName = "IOS"),
    Switch UMETA(DisplayName = "NintendoSwitch"),
};

/** @enum The different easings available */
UENUM(Blueprintable)
enum class ERyMathEasingType : uint8
{
    Linear,
    QuadraticEaseIn,
    QuadraticEaseOut,
    QuadraticEaseInOut,
    CubicEaseIn,
    CubicEaseOut,
    CubicEaseInOut,
    QuarticEaseIn,
    QuarticEaseOut,
    QuarticEaseInOut,
    QuinticEaseIn,
    QuinticEaseOut,
    QuinticEaseInOut,
    SineEaseIn,
    SineEaseOut,
    SineEaseInOut,
    CircularEaseIn,
    CircularEaseOut,
    CircularEaseInOut,
    ExponentialEaseIn,
    ExponentialEaseOut,
    ExponentialEaseInOut,
    ElasticEaseIn,
    ElasticEaseOut,
    ElasticEaseInOut,
    BackEaseIn,
    BackEaseOut,
    BackEaseInOut,
    BounceEaseIn,
    BounceEaseOut,
    BounceEaseInOut
};

#define STR_SPLIT TEXT("--- | --- | ---")
#define STR_NULLPTR TEXT("nullptr")
#define STR_BOOL_TRUE TEXT("TRUE")
#define STR_BOOL_FALSE TEXT("FALSE")
#define STR_NONE TEXT("NONE")
#define VALUE_UV_MAX 1.0f
#define VALUE_UV_MIN 0.0f
#define TO_STR_BOOL(State) (State ? STR_BOOL_TRUE : STR_BOOL_FALSE)

/** @struct Data on structures for drawing arrow directions **/
USTRUCT(BlueprintType)
struct FDirectionArrowDebugData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ArrowDistance{100.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ArrowSize{10.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor ForwardArrowColor{FColor::Red};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor InvForwardArrowColor{FColor::Red};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor RightArrowColor{FColor::Green};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor InvRightArrowColor{FColor::Green};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Thickness{1.0f};
};

/** @struct data for drawing the box size of the collision **/
USTRUCT(BlueprintType)
struct FBoxActorDebugData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector DefaultSizeBox{FVector(10.0f)};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor BoxColor{FColor::Orange};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Thickness{1.0f};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFinishResultSimpleMoveSignature, FVector, TargetPosition, bool, bIsSuccess);

UENUM(BlueprintType)
enum class ESideAxis : uint8
{
    None UMETA(Hidden),
    X,
    Y,
    Z
};

/** @enum Physical speed in units **/
UENUM(BlueprintType)
enum class ESpeedUnit : uint8
{
    /* Centimeter / second (cm/s). This is default unreal velocity unit. */
    CentimeterPerSecond		UMETA(DisplayName = "CentimeterPerSecond"),

    /* Foot / second (ft/s). */
    FootPerSecond			UMETA(DisplayName = "FootPerSecond"),

    /* Meter / second (m/s). */
    MeterPerSecond			UMETA(DisplayName = "MeterPerSecond"),

    /* Meter / minute (m/min). */
    MeterPerMinute			UMETA(DisplayName = "MeterPerMinute"),

    /* Kilometer / second (km/s). */
    KilometerPerSecond		UMETA(DisplayName = "KilometerPerSecond"),

    /* Kilometer / minute (km/min). */
    KilometerPerMinute		UMETA(DisplayName = "KilometerPerMinute"),

    /*Kilometer / hour (km/h). */
    KilometerPerHour		UMETA(DisplayName = "KilometerPerHour"),

    /* Mile / hour (mph). */
    MilePerHour				UMETA(DisplayName = "MilePerHour"),

    /* Knot (kn). Nautical mile per hour. */
    Knot					UMETA(DisplayName = "Knot"),

    /* Mach (speed of sound) (M) at standard atm. */
    Mach					UMETA(DisplayName = "Mach"),

    /* Speed of light. */
    SpeedOfLight			UMETA(DisplayName = "SpeedOfLight"),

    /* Yard / second. */
    YardPerSecond			UMETA(DisplayName = "YardPerSecond")
};

UENUM(BlueprintType)
enum class EThreadTaskType : uint8
{
    GameThread  UMETA(DisplayName = "GameThread"),
    AnyThread   UMETA(DisplayName = "AnyThread")
};


UENUM(BlueprintType)
enum class EThreadPriorityType : uint8
{
    Normal          UMETA(DisplayName = "Normal"),
    AboveNormal     UMETA(DisplayName = "AboveNormal"),
    BelowNormal     UMETA(DisplayName = "BelowNormal"),
    Highest         UMETA(DisplayName = "Highest"),
    Lowest          UMETA(DisplayName = "Lowest")
};

UENUM(BlueprintType)
enum class ETickSplited_Exec : uint8
{
    Loop        UMETA(DisplayName = "Loop"),
    Complete    UMETA(DisplayName = "Complete")
};


DECLARE_DYNAMIC_DELEGATE(FBPAsyncSignature);
DECLARE_DYNAMIC_DELEGATE_OneParam(FBPParallelForSignature, int32, Val);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleAsyncCallSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncCallCompleteSignature);

UENUM(BlueprintType)
enum class EAsyncCallType : uint8
{
    MainThread              UMETA(DisplayName = "MainThread"),
    Thread                  UMETA(DisplayName = "Thread"),
    ThreadPool              UMETA(DisplayName = "ThreadPool"),
    LargeThreadPool         UMETA(DisplayName = "LargeThreadPool")
};

UENUM(BlueprintType)
enum class EJsonType : uint8
{
    None,
    Null,
    String,
    Number,
    Boolean,
    Array,
    Object
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "BlueprintJson.BlueprintJsonLibrary.JsonMake", HasNativeBreak = "BlueprintJson.BlueprintJsonLibrary.Conv_JsonObjectToString"))
struct FBlueprintJsonObject
{
    GENERATED_USTRUCT_BODY()

    TSharedPtr<class FJsonObject> Object;
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "BlueprintJson.BlueprintJsonLibrary.JsonMakeString"))
struct FBlueprintJsonValue
{
    GENERATED_USTRUCT_BODY()

    TSharedPtr<class FJsonValue> Value;
};
