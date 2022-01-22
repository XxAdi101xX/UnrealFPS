// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "Components/BoxComponent.h"
#include "MainCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
    DamageCollision->SetupAttachment(RootComponent);
    
    AIPerComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    
    SightConfig->SightRadius = 1250.0f;
    SightConfig->LoseSightRadius = 1250.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->SetMaxAge(0.1f); // The time it takes for the enemy to forget the detected player
    
    AIPerComp->ConfigureSense(*SightConfig);
    AIPerComp->SetDominantSense(SightConfig->GetSenseImplementation());
    AIPerComp->OnPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::OnSensed);
    
    CurrentVelocity = FVector::ZeroVector;
    MovementSpeed = 375.0f;
    
    DistanceSquared = BIG_NUMBER;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
    
    DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnHit);
    
    BaseLocation = this->GetActorLocation();
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    

}

void AEnemyCharacter::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
           UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
           bool bFromSweep, const FHitResult &Hit)
{
    
    
}

void AEnemyCharacter::OnSensed(const TArray<AActor *> &UpdatedActors)
{
    
}

void AEnemyCharacter::SetNewRotation(FVector TargetPosition, FVector CurrentPosition)
{
    
}

void AEnemyCharacter::DealDamage(float DamageAmount)
{
    
}
