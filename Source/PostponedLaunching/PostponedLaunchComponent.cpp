#include "PostponedLaunchComponent.h"

TMap<int32, FInitializationQueueItem> UPostponedLaunchComponent::GlobalInitializationQueue;

int32 UPostponedLaunchComponent::GenerateUniqueID()
{
    static int32 UniqueID = 0;
    return UniqueID++;
}

UPostponedLaunchComponent::UPostponedLaunchComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPostponedLaunchComponent::AddToInitializationQueue(const InitDelegate& Delegate, bool bGlobalDelay)
{
    if (CompleteWithoutDelay || !CheckGameStarted() || IsOwnedByPlayer() || !UsedGlobalDelay(bGlobalDelay))
    {
        Delegate.Execute();
    }
    else
    {
        const FInitializationQueueItem NewItem = { Delegate, GetOwner(),
            bGlobalDelay ? EQueueItemState::GlobalPending : EQueueItemState::Local };
        
        LocalInitializationQueue.Add(GenerateUniqueID(), NewItem);
    }
}


bool UPostponedLaunchComponent::UsedGlobalDelay(bool bCurrentGlobal)
{
    return bUsedGlobalDelay = bUsedGlobalDelay || bCurrentGlobal;
}

int32 UPostponedLaunchComponent::ActiveComponentsCount = 0;

void UPostponedLaunchComponent::BeginPlay()
{
    Super::BeginPlay();

    if (ActiveComponentsCount == 0)
    {
        ClearGlobalInitializationQueue();
    }

    ActiveComponentsCount++;  
}

void UPostponedLaunchComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    ActiveComponentsCount--;  
    
    if (ActiveComponentsCount <= 0)
    {
        ClearGlobalInitializationQueue();
    }
}


void UPostponedLaunchComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (LocalInitializationQueue.IsEmpty())
    {
        if (ActiveComponentsCount <= 1)
        {
            ClearGlobalInitializationQueue();
        }
        return;
    }

    if (!LockInitProcess || CompleteWithoutDelay)
    {
        if (IsOwnedByPlayer() || CompleteWithoutDelay)
            CompleteQueueWithoutDelay();
        else
            ProcessInitializationQueue();

        ProcessGlobalInitializationQueue(DeltaTime);
    }
}

void UPostponedLaunchComponent::ProcessInitializationQueue()
{
    for (auto It = LocalInitializationQueue.CreateIterator(); It; ++It)
    {
        FInitializationQueueItem& Item = It.Value();

        switch (Item.QueueState)
        {
        case EQueueItemState::GlobalPending:
            if (!GlobalInitializationQueue.Contains(It.Key()))
            {
                Item.QueueState = EQueueItemState::SentToGlobalQueue;
                GlobalInitializationQueue.Add(It.Key(), Item);
            }
            return;

        case EQueueItemState::SentToGlobalQueue:
            if (!GlobalInitializationQueue.Contains(It.Key()))
            {
                It.RemoveCurrent();
            }
            return;

        case EQueueItemState::Local:
        default:
            ExecuteInitializationItem(Item);
            It.RemoveCurrent();
            break;
        }
    }
}

void UPostponedLaunchComponent::CompleteQueueWithoutDelay()
{
    for (auto It = LocalInitializationQueue.CreateIterator(); It; ++It)
    {
        FInitializationQueueItem& Item = It.Value();
        GlobalInitializationQueue.Remove(It.Key());
        ExecuteInitializationItem(Item);
        It.RemoveCurrent();
    }
}

void UPostponedLaunchComponent::ExecuteInitializationItem(const FInitializationQueueItem& Item)
{
    if (Item.DelegateFunction.IsBound())
    {
        Item.DelegateFunction.Execute();
    }
}

void UPostponedLaunchComponent::ProcessGlobalInitializationQueue(float DeltaTime)
{
    static float TimeSinceLastCall = 0.0f;
    TimeSinceLastCall += DeltaTime;

    if (TimeSinceLastCall >= DelayGlobalInitializationQueue
        && GlobalInitializationQueue.Num() > 0)
    {
        auto It = GlobalInitializationQueue.CreateIterator();
        ExecuteInitializationItem(It.Value());
        It.RemoveCurrent();

        TimeSinceLastCall = 0.0f;
    }
}

void UPostponedLaunchComponent::ClearGlobalInitializationQueue()
{
    GlobalInitializationQueue.Empty(); 
    ActiveComponentsCount = 0;
}

bool UPostponedLaunchComponent::IsOwnedByPlayer() const
{
    const APawn* OwnerPawn = Cast<APawn>(GetOwner());
    return OwnerPawn != nullptr && OwnerPawn->IsPlayerControlled();
}

bool UPostponedLaunchComponent::CheckGameStarted() const
{
    return GetWorld() && GetWorld()->HasBegunPlay();
}

