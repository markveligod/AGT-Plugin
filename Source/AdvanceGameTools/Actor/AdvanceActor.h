/** Copyright Mark Veligod. Published in 2023. **/

#pragma once

#include "CoreMinimal.h"
#include "AdvanceGameTools/AGTDataTypes.h"
#include "GameFramework/Actor.h"
#include "AdvanceActor.generated.h"

class USceneComponent;
class UBillboardComponent;

UCLASS()
class ADVANCEGAMETOOLS_API AAdvanceActor : public AActor
{
    GENERATED_BODY()

#pragma region DefaultInterface

public:
    // Sets default values for this actor's properties
    AAdvanceActor();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    /**
     * Called when an instance of this class is placed (in editor) or spawned.
     * @param	Transform			The transform the actor was constructed at.
     */
    virtual void OnConstruction(const FTransform& Transform) override;

    /** @protected Pre editor tick**/
    void PreEditorTick(float DeltaTime);

    /** @protected Editor tick **/
    virtual void EditorTick(float DeltaTime);

    /** @protected Editor tick event**/
    UFUNCTION(BlueprintImplementableEvent)
    void EditorTick_Event(float DeltaTime);

#pragma endregion

#pragma region ActionDrawDebug

private:
    /** @private Drawing actor directions **/
    void DrawArrowDirectionActor(float DeltaTime) const;

    /** @private relative position of the actor label **/
    void ReplaceBillboardPosition() const;

    /** @private Drawing the Actor collision box **/
    void DrawBoxActor(float DeltaTime) const;

#pragma endregion

#pragma region Components

protected:
    /** @protected The basic pseudo root component **/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AdvanceActorSettings")
    TObjectPtr<USceneComponent> RootScene;

    /** @protected Actor label for actor selection **/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AdvanceActorSettings")
    TObjectPtr<UBillboardComponent> BillboardScene;

#pragma endregion

#pragma region Data

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvanceActorSettings")
    bool bEnableEditorTick{true};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvanceActorSettings", meta = (ClampMin = "1.0", ForceUnits = "x"))
    float TickRatePerSecond{60.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvanceActorSettings", meta = (ConsoleVariable = "AdvanceActor.EnableDrawArrowActor"))
    bool bEnableDrawArrowActor{false};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvanceActorSettings", meta = (EditCondition = "bEnableDrawArrowActor", EditConditionHides))
    FDirectionArrowDebugData DirectionArrowDebugData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvanceActorSettings", meta = (ConsoleVariable = "AdvanceActor.EnableDrawBoxActor"))
    bool bEnableDrawBoxActor{false};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvanceActorSettings", meta = (EditCondition = "bEnableDrawBoxActor", EditConditionHides))
    FBoxActorDebugData BoxActorDebugData;

private:
    /** @private The structure of the timer for calling the editor's tick **/
    FTimerHandle EditorTickTimerHandle;

#pragma endregion
};
