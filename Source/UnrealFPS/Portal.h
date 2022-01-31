// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class UNREALFPS_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    
    // Determines whether the portal is active
    UFUNCTION(BlueprintPure, Category = Portal)
    bool IsActive();
    
    // Set active state TODO: currently unused
    UFUNCTION(BluePrintCallable, Category = Portal)
    void SetActive(bool NewActiveState);
    
    // Set the render target texture
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Portal)
    void SetRenderTargetTexture(UTexture *RenderTexture);
    
    // Clear the render target texture
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Portal)
    void ClearRenderTargetTexture();
    
    // Get the target portal
    UFUNCTION(BlueprintPure, Category = Portal)
    AActor *GetTarget();
    
    // Check if the position is in front of the portal
    UFUNCTION(BlueprintCallable, Category = Portal)
    bool IsLocationInFront(FVector Location, FVector PortalLocation, FVector PortalNormal);

    // Check if the point has crossed the portal
    UFUNCTION(BlueprintCallable, Category = Portal)
    bool HasLocationCrossed(FVector NewLocation, FVector PortalLocation, FVector PortalNormal);

    // Teleport the actor to the portal
    UFUNCTION(BlueprintCallable, Category = Portal)
    void TeleportActor(AActor *ActorToTeleport);
    
    // TODO: move these to generic helper class
    UFUNCTION(BlueprintPure, Category = Portal)
    bool IsPointInsideBox( FVector Point, UBoxComponent* Box );
    
    FVector ConvertLocationToActorSpace(FVector Location, AActor *ReferenceActor, AActor *TargetActor);
    
    FRotator ConvertRotationToActorSpace(FRotator Rotation, AActor *ReferenceActor, AActor *TargetActor);

protected:
    UPROPERTY(BlueprintReadOnly)
    USceneComponent* PortalRootComponent;

private:
    // TODO: this and the setter function is not being used currently, should be addressed
    bool bIsActive;

    APortal *TargetPortal;

    // TODO: Last location is currently uninitialized so we want to get the position of the player before start
    // we should also generalize this to work with any actor hence this last position should be a variable on the actor itself?
    FVector LastLocation;
};
