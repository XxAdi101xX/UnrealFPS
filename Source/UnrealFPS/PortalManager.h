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
    // Called every frame
    //virtual void Tick(float DeltaTime) override;
    
	// Sets default values for this actor's properties
	APortalManager();

    // Save a reference to the Player
    void SetPlayer(AMainCharacter *NewPlayer);

    // Various setup that happens during spawn
    void Init();

    // Manual Tick // TODO: where do we call this?
    void Update(float DeltaTime);

    // Update SceneCapture
    void UpdateCapture(APortal* Portal);

private:
    //Function to create the Portal render target
    void GeneratePortalTexture();

    UPROPERTY()
    USceneCaptureComponent2D *SceneCapture;

    UPROPERTY(transient)
    UTextureRenderTarget2D *PortalTexture;

    UPROPERTY()
    AMainCharacter *CharacterOwner;

    int32 PreviousScreenSizeX;
    int32 PreviousScreenSizeY;

    float UpdateDelay;

};
