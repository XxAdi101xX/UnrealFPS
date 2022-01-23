// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameMode.h"

#include "Kismet/GameplayStatics.h"

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
    UGameplayStatics::OpenLevel(GetWorld(), "MainLevel");
}

void AFPSGameMode::CountDownTimer()
{
    if (--TimerCount == 0)
    {
        GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
        ResetLevel();
    }
}
