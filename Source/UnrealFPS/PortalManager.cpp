// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalManager.h"

#include "MainCharacter.h"
#include "Portal.h"
#include "EngineUtils.h"

APortalManager::APortalManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APortalManager::BeginPlay()
{
	Super::BeginPlay();
}

void APortalManager::SetPlayer(AMainCharacter *NewPlayer)
{
    CharacterOwner = NewPlayer;
}

void APortalManager::Update(float DeltaTime)
{
    for (TActorIterator<APortal> PortalIterator(GetWorld()); PortalIterator; ++PortalIterator)
    {
        PortalIterator->ClearRenderTargetTexture();
        PortalIterator->UpdateCapture(CharacterOwner->GetCameraProjectionMatrix());
    }
}


