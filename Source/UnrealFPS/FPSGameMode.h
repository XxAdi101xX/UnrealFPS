// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FPSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UNREALFPS_API AFPSGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
    
    AFPSGameMode();
    
    void RestartGameplay(bool Won);
    
    UPROPERTY(BlueprintReadOnly)
    int TimerCount = 300;
    
    void BeginPlay() override;
    
private:
    void ResetLevel();
    
    FTimerHandle CountDownTimerHandle = FTimerHandle();
    
    void CountDownTimer();
};
