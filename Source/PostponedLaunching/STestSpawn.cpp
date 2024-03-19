
#include "STestSpawn.h"

#include "GameFramework/Character.h"


ASTestSpawn::ASTestSpawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASTestSpawn::BeginPlay()
{
	Super::BeginPlay();

	if (!CharacterToSpawn) return;
    
	if (bSpawnContinuously)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASTestSpawn::ContinuousSpawn, TimeDelay, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASTestSpawn::SpawnCharacters, TimeDelay, false);
	}
}

void ASTestSpawn::SpawnCharacters()
{
	for (int i = 0; i < CountSpawn; ++i) 
	{
		const FVector2D RandCirclePoint = FMath::RandPointInCircle(SpawnRadius);
		FVector SpawnLocation = GetActorLocation() + FVector(RandCirclePoint.X, RandCirclePoint.Y, 0.0f);

		FRotator SpawnRotation = FRotator::ZeroRotator;
		
		GetWorld()->SpawnActor<ACharacter>(CharacterToSpawn, SpawnLocation, SpawnRotation);
	}
}

void ASTestSpawn::ContinuousSpawn()
{
	if (IndexSpawn < CountSpawn) 
	{
		const FVector2D RandCirclePoint = FMath::RandPointInCircle(SpawnRadius);
		const FVector SpawnLocation = GetActorLocation() + FVector(RandCirclePoint.X, RandCirclePoint.Y, 0.0f);
		const FRotator SpawnRotation = FRotator::ZeroRotator;

		GetWorld()->SpawnActor<ACharacter>(CharacterToSpawn, SpawnLocation, SpawnRotation);

		IndexSpawn++;

		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASTestSpawn::ContinuousSpawn, TimeDelayBetweenSpawn, false);
	}
}

