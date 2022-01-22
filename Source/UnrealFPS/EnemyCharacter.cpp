// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "Components/BoxComponent.h"
#include "MainCharacter.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
    DamageCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
    
    DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnHit);
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

UFUNCTION()
void AEnemyCharacter::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
           UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
           bool bFromSweep, const FHitResult &Hit)
{
    
}
