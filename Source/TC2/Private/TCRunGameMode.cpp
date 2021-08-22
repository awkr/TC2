// Fill out your copyright notice in the Description page of Project Settings.

#include "TCRunGameMode.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "TCPlayerController.h"
#include "TCPlayerState.h"
#include "TC2/Log.h"
#include "TCFloor.h"
#include "TCWater.h"

ATCRunGameMode::ATCRunGameMode() : Super()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
            TEXT("/Game/Blueprints/BP_TCCharacter.BP_TCCharacter_C"));
    DefaultPawnClass = PlayerPawnClassFinder.Class;

    static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(
            TEXT("/Game/Blueprints/BP_TCPlayerController.BP_TCPlayerController_C"));
    PlayerControllerClass = PlayerControllerClassFinder.Class;

    PlayerStateClass = ATCPlayerState::StaticClass();

    // init speed rules
    SpeedRules.Add(FSpeedRule(10, 1));
    SpeedRules.Add(FSpeedRule(8, 1.2));
    SpeedRules.Add(FSpeedRule(8, 1.3));
    SpeedRules.Add(FSpeedRule(8, 1.4));
    SpeedRules.Add(FSpeedRule(8, 1.6));
    SpeedRules.Add(FSpeedRule(8, 1.8));
}

void ATCRunGameMode::BeginPlay()
{
    LOG_FUNC_WARN()
    Super::BeginPlay();

    // load floor(s)
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATCFloor::StaticClass(), FoundActors);
    FoundActors.Sort([](const AActor& A, const AActor& B)
    {
        return A.GetTransform().GetLocation().X < B.GetTransform().GetLocation().X;
    });
    if (FoundActors.Num() == 0) {
        LOG_ERROR("No floor found")
    }
    for (AActor* Actor : FoundActors)
    {
        if (ATCFloor* Floor = Cast<ATCFloor>(Actor))
        {
            FloorAndWater.Add(Floor);

            FloorSize = Floor->GetScaledBoundingBox();
        }
    }

    // load water(s)
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATCWater::StaticClass(), FoundActors);
    FoundActors.Sort([](const AActor& A, const AActor& B)
    {
        return A.GetTransform().GetLocation().X < B.GetTransform().GetLocation().X;
    });
    if (FoundActors.Num() == 0) {
        LOG_ERROR("No water found")
    }
    for (AActor* Actor : FoundActors)
    {
        if (ATCWater* Water = Cast<ATCWater>(Actor))
        {
            FloorAndWater.Add(Water);

            WaterSize = Water->GetScaledBoundingBox();
        }
    }

    // load obstacle(s)
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATCObstacle::StaticClass(), FoundActors);
    FoundActors.Sort([](const AActor& A, const AActor& B)
    {
        return A.GetTransform().GetLocation().X < B.GetTransform().GetLocation().X;
    });
    if (FoundActors.Num() == 0) {
        LOG_ERROR("No obstacle found")
    }
    for (AActor* Actor : FoundActors)
    {
        if (ATCObstacle* Obstacle = Cast<ATCObstacle>(Actor))
        {
            Obstacles.Add(Obstacle);
        }
    }

    //
    StartAt = UGameplayStatics::GetTimeSeconds(GetWorld());
}

void ATCRunGameMode::SpawnFloor()
{
    if (!FloorAndWater.Last()->IsA(ATCWater::StaticClass()) && FMath::RandBool()) // if spawn water
    {
        SpawnWater();
    }

    ATCCuboid* LastActor = FloorAndWater.Last();
    const float LastX = LastActor->GetTransform().GetLocation().X;
    const FVector Size = LastActor->GetScaledBoundingBox();
    const float X = LastX + Size.X / 2 + FloorSize.X / 2;
    const FVector Location(X, 0, 0);

    const FRotator Rotation = LastActor->GetTransform().Rotator();

    FActorSpawnParameters Parameters;
    Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ATCFloor* Floor = GetWorld()->SpawnActor<ATCFloor>(Location, Rotation, Parameters);
    FloorAndWater.Add(Floor);

    SpawnBuilding();

    SpawnObstacle(2);
}

void ATCRunGameMode::SpawnWater()
{
    const ATCCuboid* LastActor = FloorAndWater.Last();
	const float LastX = LastActor->GetTransform().GetLocation().X;
	const FVector Size = LastActor->GetScaledBoundingBox();
	const float X = LastX + Size.X / 2 + WaterSize.X / 2;

	const FVector Location(X, 0, -60.0);

	const FRotator Rotation = LastActor->GetTransform().Rotator();

    FActorSpawnParameters Parameters;
    Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATCWater* Water = GetWorld()->SpawnActor<ATCWater>(Location, Rotation, Parameters);
	FloorAndWater.Add(Water);
}

void ATCRunGameMode::SpawnBuilding() // spawn 1-3 buildings at on side
{
	const int Num = FMath::RandRange(1, 3);

	const ATCCuboid* LastActor = FloorAndWater.Last();
	const FVector Size = LastActor->GetScaledBoundingBox();
	const float Length = Size.X;
	const float LastX = LastActor->GetTransform().GetLocation().X;
	const float LastY = LastActor->GetTransform().GetLocation().Y;

	const float AvgLength = Length / Num;
	const float FirstStartX = LastX - Length / 2;
	const float FirstEndX = FirstStartX + AvgLength;

	for (uint8 Index = 0; Index < Num; ++Index)
	{
		const float Len = FMath::FRandRange(200, AvgLength - 250); // whatever you like

		// a little gap between each building or floor's X edge
		const float StartX = FirstStartX + AvgLength * Index + Len / 2 + 60;
		const float EndX = FirstEndX + AvgLength * Index - Len / 2 - 50;

		const float Width = FMath::FRandRange(150, 300); // whatever you like
        const float HeightScale = FMath::FRandRange(200, 1000);

		const float X = FMath::FRandRange(StartX, EndX);
		const float Y = LastY + Size.Y / 2 - 60 /* a little gap */ - Width / 2;
		const FVector Location(X, Y, HeightScale / 2);

		const FRotator Rotation = LastActor->GetTransform().Rotator();
		const FVector Scale = FVector(Len / 100, Width / 100, HeightScale / 100);
		FTransform Transform(Rotation, Location, Scale);

        ATCBuilding* Building = GetWorld()->SpawnActorDeferred<ATCBuilding>(
			ATCBuilding::StaticClass(), Transform);
		Building->FinishSpawning(Transform);
		Buildings.Add(Building);
	}
}

void ATCRunGameMode::SpawnObstacle(const int Num)
{
	for (int8 i = 0; i < Num; ++i)
	{
        const ATCObstacle* LastObstacle = Obstacles.Last();
		float LastX = LastObstacle->GetTransform().GetLocation().X;

		const float X = FMath::RandRange(900, 1800);
		const FVector Location(LastX + X, 0, 0);
		const FRotator Rotation = LastObstacle->GetTransform().Rotator();
		const FVector Scale = FVector(FMath::FRandRange(.2, .5), FMath::FRandRange(6, 8), FMath::FRandRange(.75, 1.9));
		const float Z = Scale.Z / 2 * 100;

		FTransform Transform(Rotation, FVector(Location.X, Location.Y - 150, Z), Scale);

		ATCObstacle* Obstacle = GetWorld()->SpawnActorDeferred<ATCObstacle>(
			ATCObstacle::StaticClass(), Transform);
		Obstacle->FinishSpawning(Transform);

		Obstacles.Add(Obstacle);
	}
}
