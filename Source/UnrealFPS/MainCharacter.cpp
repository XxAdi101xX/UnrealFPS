// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

#include "Projectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"

#include "FPSGameMode.h"
#include "PortalManager.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    GetCapsuleComponent()->InitCapsuleSize(40.0f, 95.0f);
    
    TurnRate = 45.0f;
    LookUpRate = 45.0f;
    
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->AddRelativeLocation(FVector(-39.65f, 1.75f, 64.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;
    
    HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
    HandsMesh->SetOnlyOwnerSee(true);
    HandsMesh->SetupAttachment(FirstPersonCamera);
    HandsMesh->bCastDynamicShadow = false;
    HandsMesh->CastShadow = false;
    HandsMesh->AddRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
    HandsMesh->AddRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
    
    GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
    GunMesh->SetOnlyOwnerSee(true);
    GunMesh->bCastDynamicShadow = false;
    GunMesh->CastShadow = false;
    
    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Location"));
    MuzzleLocation->SetupAttachment(GunMesh);
    MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.44f, -10.6f));
    
    GunOffset = FVector(100.0f, 0.0f, 10.0f);

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    GunMesh->AttachToComponent(HandsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GripPoint"));
    
    World = GetWorld();
    
    AnimInstance = HandsMesh->GetAnimInstance();
    
    // Create the Portal Manager;
    FActorSpawnParameters SpawnParams;

    PortalManager = GetWorld()->SpawnActor<APortalManager>(APortalManager::StaticClass(),
                                                          FVector::ZeroVector,
                                                          FRotator::ZeroRotator,
                                                          SpawnParams);
    PortalManager->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
    PortalManager->SetPlayer(this);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    PortalManager->Update(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMainCharacter::OnFire);
    
    PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
    
    PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookAtRate);
}


void AMainCharacter::OnFire()
{
    if (World != nullptr)
    {
        SpawnRotation = GetControlRotation();
        
        SpawnLocation = (MuzzleLocation != nullptr ? MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
        
        // TODO: this might not be want we want because we might not want the bullet being spawned elsewhere
        FActorSpawnParameters ActorSpawnParams;
        ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
        
        World->SpawnActor<AProjectile>(Projectile, SpawnLocation, SpawnRotation, ActorSpawnParams);
        
        if (FireSound != nullptr)
        {
            UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
        }
        
        if (FireAnimation != nullptr && AnimInstance != nullptr)
        {
            AnimInstance->Montage_Play(FireAnimation, 1.0f);
        }
    }
}

void AMainCharacter::MoveForward(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

void AMainCharacter::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}

void AMainCharacter::TurnAtRate(float Rate)
{
    AddControllerYawInput(Rate * TurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookAtRate(float Rate)
{
    AddControllerPitchInput(Rate * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::DealDamage(float DamageAmount)
{
    Health -= DamageAmount;
    
    if (Health <= 0.0f)
    {
        // Restart game
        AFPSGameMode *GameMode = Cast<AFPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        
        if (GameMode)
        {
            GameMode->RestartGameplay(false);
        }
        
        Destroy();
    }
}

FMatrix AMainCharacter::GetCameraProjectionMatrix()
{
    FMatrix ProjectionMatrix;

    if( UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer() != nullptr )
    {
        FSceneViewProjectionData PlayerProjectionData;

        UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer()->GetProjectionData( UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer()->ViewportClient->Viewport,
                                        EStereoscopicPass::eSSP_FULL,
                                        PlayerProjectionData );

        ProjectionMatrix = PlayerProjectionData.ProjectionMatrix;
    }

    return ProjectionMatrix;
}


