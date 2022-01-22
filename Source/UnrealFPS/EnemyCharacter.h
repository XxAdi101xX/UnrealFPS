// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class UNREALFPS_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    UPROPERTY(EditAnywhere)
    class UBoxComponent *DamageCollision;
    
    UFUNCTION()
    void OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
               UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
               bool bFromSweep, const FHitResult &Hit);
    
    
    UPROPERTY(VisibleDefaultsOnly, Category = Enemy)
    class UAIPerceptionComponent *AIPerComp;
    
    UPROPERTY(VisibleDefaultsOnly, Category = Enemy)
    class UAISenseConfig_Sight *SightConfig;
    
    UFUNCTION()
    void OnSensed(const TArray<AActor *> &UpdatedActors);
    
    UPROPERTY(VisibleAnywhere, Category = Movement)
    FRotator EnemyRotation;
    
    UPROPERTY(VisibleAnywhere, Category = Movement)
    FVector BaseLocation;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    FVector CurrentVelocity;
    
    UPROPERTY(VisibleAnywhere, Category = Movement)
    float MovementSpeed;
    
    void SetNewRotation(FVector TargetPosition, FVector CurrentPosition);
    
    bool BackToBaseLocation;
    FVector NewLocation;
    float DistanceSquared;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Health = 100.0f;
    
    UPROPERTY(EditAnywhere)
    float DamageValue = 5.0f;
    
public:
    void DealDamage(float DamageAmount);
};
