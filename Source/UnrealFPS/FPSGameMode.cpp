// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameMode.h"

#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"


AFPSGameMode::AFPSGameMode() : AGameMode()
{
    // See https://answers.unrealengine.com/questions/486440/how-to-use-the-player-controller-class-in-c.html for more details
    
    //PlayerControllerClass = ACustomPlayerController::StaticClass();
    DefaultPawnClass = AMainCharacter::StaticClass();
    //GameStateClass = ACustomGameState::StaticClass();
    //HUDClass = ACustomGameHUD::StaticClass();
    //ReplaySpectatorPlayerControllerClass = ACustomReplaySpectatorPlayerController::StaticClass();
    //SpectatorClass = ACustomSpectatorClass::StaticClass();
}

void AFPSGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &AFPSGameMode::CountDownTimer, 1.0f, true, 1.0f);
}

void AFPSGameMode::RestartGameplay(bool Won)
{
    if (Won)
    {
        ResetLevel();
    }
    else
    {
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AFPSGameMode::ResetLevel, 3.0f);
    }
}

void AFPSGameMode::ResetLevel()
{
    UGameplayStatics::OpenLevel(GetWorld(), "FirstPersonExampleMap");
}

void AFPSGameMode::CountDownTimer()
{
    if (--TimerCount == 0)
    {
        GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
        ResetLevel();
    }
}
