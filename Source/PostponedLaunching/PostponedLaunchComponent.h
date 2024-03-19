#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PostponedLaunchComponent.generated.h"

// Macro for simplifying the queue addition syntax
#define ADD_TO_LOCAL_INIT_QUEUE(FunctionName) \
AddToInitializationQueue(InitDelegate::CreateUObject(this, &ThisClass::FunctionName), false)

#define ADD_TO_GLOBAL_INIT_QUEUE(FunctionName) \
AddToInitializationQueue(InitDelegate::CreateUObject(this, &ThisClass::FunctionName), true)


// Delegate type for initialization functions
DECLARE_DELEGATE(InitDelegate);

UENUM(BlueprintType)
enum class EQueueItemState : uint8
{
	Local UMETA(DisplayName = "Local"),
	GlobalPending UMETA(DisplayName = "Global Pending"),
	SentToGlobalQueue UMETA(DisplayName = "Sent To Global Queue")
};

// Structure for queue items
USTRUCT()
struct FInitializationQueueItem
{
	GENERATED_BODY()

	InitDelegate DelegateFunction;
	TWeakObjectPtr<AActor> OwningActor;
	EQueueItemState QueueState;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POSTPONEDLAUNCHING_API UPostponedLaunchComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPostponedLaunchComponent();

	void AddToInitializationQueue(const InitDelegate& Delegate, bool bGlobalDelay);
	bool UsedGlobalDelay(bool bCurrentDelay);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ProcessInitializationQueue();
	virtual void CompleteQueueWithoutDelay();

	static void ExecuteInitializationItem(const FInitializationQueueItem& Item);
	static void ProcessGlobalInitializationQueue(float DeltaTime);
	static void ClearGlobalInitializationQueue();

	bool IsOwnedByPlayer() const;
	bool CheckGameStarted() const;
	static int32 GenerateUniqueID();
	
	TMap<int32, FInitializationQueueItem> LocalInitializationQueue;
	static TMap<int32, FInitializationQueueItem> GlobalInitializationQueue;
	inline static float DelayGlobalInitializationQueue = 2.f;
	
	static int32 ActiveComponentsCount;

	bool bUsedGlobalDelay{ false };
	bool LockInitProcess{ false };
	bool CompleteWithoutDelay{ false };
};
