
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STestSpawn.generated.h"

UCLASS()
class POSTPONEDLAUNCHING_API ASTestSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTestSpawn();
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TSubclassOf<ACharacter> CharacterToSpawn {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 CountSpawn {10};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	float TimeDelay {3.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	float TimeDelayBetweenSpawn {0.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	bool bSpawnContinuously {false}; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	float SpawnRadius {300.0f};

	void SpawnCharacters();
	void ContinuousSpawn();  

private:
	FTimerHandle SpawnTimerHandle;
	int32 IndexSpawn {0};
};