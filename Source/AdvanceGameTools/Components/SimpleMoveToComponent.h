/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "AdvanceGameTools/AGTDataTypes.h"
#include "Components/ActorComponent.h"
#include "SimpleMoveToComponent.generated.h"

class USplineComponent;

UCLASS(ClassGroup = (AIManager))
class ADVANCEGAMETOOLS_API USimpleMoveToComponent : public UActorComponent
{
    GENERATED_BODY()

#pragma region Signature

public:
    UPROPERTY(BlueprintAssignable)
    FFinishResultSimpleMoveSignature OnFinishResultSimpleMove;

#pragma endregion

#pragma region Default

public:
    // Sets default values for this component's properties
    USimpleMoveToComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

#pragma region Components

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USplineComponent> Spline;

#pragma endregion

#pragma region ActionMove

public:
    /** @public Starting the owner's movement to a certain position **/
    UFUNCTION(BlueprintCallable, Category = "ActionMove")
    void StartSimpleMove(FVector TargetPos);

private:
    /** @private Registration of the completion of the owner's move to the coordinates **/
    void RegisterRequestFinished_Event(bool bIsSuccess);

    /** @private **/
    void ResetSimpleMove();

#pragma endregion

#pragma region DataMove

private:
    FVector Goal{FVector::ZeroVector};
    FVector NextPoint{FVector::ZeroVector};
    float DistanceToGoal{0.0f};
    float Length{0.0f};
    float CurrentTime{0.0f};
    float GoalDistanceThreshold{100.0f};
    float DistanceThreshold{50.0f};
    float DistanceBetweenPoints{100.0f};

#pragma endregion
};
