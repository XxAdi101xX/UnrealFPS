// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalManager.generated.h"

class AMainCharacter;
class APortal;
class UTextureRenderTarget2D;


// TODO: does this need to extend Actor????
UCLASS()
class UNREALFPS_API APortalManager : public AActor
{
	GENERATED_BODY()
	
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	APortalManager();

    // Save a reference to the Player
    void SetPlayer(AMainCharacter *NewPlayer);

    // Manual Tick called in the MainCharacter tick() method to ensure it's run after all of the calculation for the main character
    void Update(float DeltaTime);

private:
    AMainCharacter *CharacterOwner;

};
