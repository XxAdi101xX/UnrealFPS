// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"

#include "MainCharacter.h"
#include "Portal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineUtils.h"

APortalManager::APortalManager()
{
	PrimaryActorTick.bCanEverTick = false;
    
    PortalTexture = nullptr;
    PreviousScreenSizeX = 0;
    PreviousScreenSizeY = 0;
}

void APortalManager::BeginPlay()
{
	Super::BeginPlay();
}

void APortalManager::SetPlayer(AMainCharacter *NewPlayer)
{
    CharacterOwner = NewPlayer;
}

void APortalManager::Init()
{
    // Create screen capture
    SceneCapture = NewObject<USceneCaptureComponent2D>(this, USceneCaptureComponent2D::StaticClass(), *FString("PortalSceneCapture"));

    SceneCapture->AttachToComponent( GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale );
    SceneCapture->RegisterComponent();

    SceneCapture->bCaptureEveryFrame            = false;
    SceneCapture->bCaptureOnMovement            = false;
    SceneCapture->LODDistanceFactor             = 3; //Force bigger LODs for faster computations
    SceneCapture->TextureTarget                 = nullptr;
    SceneCapture->bEnableClipPlane              = true;
    SceneCapture->bUseCustomProjectionMatrix    = true;
    SceneCapture->CaptureSource                 = ESceneCaptureSource::SCS_SceneColorHDRNoAlpha;

    //Setup Post-Process of SceneCapture (optimization : disable Motion Blur, etc)
    FPostProcessSettings CaptureSettings;

    CaptureSettings.bOverride_AmbientOcclusionQuality       = true;
    CaptureSettings.bOverride_MotionBlurAmount              = true;
    CaptureSettings.bOverride_SceneFringeIntensity          = true;
    CaptureSettings.bOverride_GrainIntensity                = true;
    CaptureSettings.bOverride_ScreenSpaceReflectionQuality  = true;

    CaptureSettings.AmbientOcclusionQuality         = 0.0f; //0=lowest quality..100=maximum quality
    CaptureSettings.MotionBlurAmount                = 0.0f; //0 = disabled
    CaptureSettings.SceneFringeIntensity            = 0.0f; //0 = disabled
    CaptureSettings.GrainIntensity                  = 0.0f; //0 = disabled
    CaptureSettings.ScreenSpaceReflectionQuality    = 0.0f; //0 = disabled

    CaptureSettings.bOverride_ScreenPercentage      = true;
    CaptureSettings.ScreenPercentage                = 100.0f;

    SceneCapture->PostProcessSettings = CaptureSettings;

    GeneratePortalTexture();
}

// Manual Tick
void APortalManager::Update(float DeltaTime)
{
    for (TActorIterator<APortal> PortalIterator(GetWorld()); PortalIterator; ++PortalIterator)
    {
        PortalIterator->ClearRenderTargetTexture();
        UpdateCapture(*PortalIterator);
    }
}

// Update SceneCapture
void APortalManager::UpdateCapture(APortal *Portal)
{
    if (CharacterOwner == nullptr)
    {
        return;
    }

    // Update SceneCapture (discard if there is no active portal)
    if (SceneCapture != nullptr && PortalTexture != nullptr && Portal != nullptr)
    {
        APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        AActor* Target  = Portal->GetTarget();

        //Place the SceneCapture to the Target
        if (Target != nullptr)
        {
            // Compute new location in the space of the target actor (which may not be aligned to world)
            FVector NewLocation = Portal->ConvertLocationToActorSpace(PlayerCameraManager->GetCameraLocation(), Portal, Target);

            SceneCapture->SetWorldLocation(NewLocation);

            // Compute new Rotation in the space of the target location
            FRotator CameraRotation  = PlayerCameraManager->GetCameraRotation();
            FTransform SourceTransform  = Portal->GetActorTransform();
            FTransform TargetTransform  = Target->GetActorTransform();

            FQuat LocalQuat             = SourceTransform.GetRotation().Inverse() * CameraRotation.Quaternion();
            FQuat NewWorldQuat          = TargetTransform.GetRotation() * LocalQuat;

            //Update SceneCapture rotation
            SceneCapture->SetWorldRotation(NewWorldQuat);

            // Add a clip plane to ignore objects between the SceneCapture and the TargetPortal when rendering the portal texture
            SceneCapture->ClipPlaneNormal   = Target->GetActorForwardVector();
            SceneCapture->ClipPlaneBase     = Target->GetActorLocation() + (SceneCapture->ClipPlaneNormal * -617.5f); // Offset to compensate the clipping plane being too far into the actual view
        }

        // Assign the Render Target
        Portal->SetRenderTargetTexture( PortalTexture );
        SceneCapture->TextureTarget = PortalTexture;

        // Get the Projection Matrix
        SceneCapture->CustomProjectionMatrix = CharacterOwner->GetCameraProjectionMatrix();

        SceneCapture->CaptureScene();
    }
}


void APortalManager::GeneratePortalTexture()
{
    int32 CurrentSizeX = 1920;
    int32 CurrentSizeY = 1080;

    if (CharacterOwner != nullptr)
    {
        UGameplayStatics::GetPlayerController(CharacterOwner, 0)->GetViewportSize(CurrentSizeX, CurrentSizeY);
    }


    if (CurrentSizeX == PreviousScreenSizeX && CurrentSizeY == PreviousScreenSizeY)
    {
        return;
    }

    PreviousScreenSizeX = CurrentSizeX;
    PreviousScreenSizeY = CurrentSizeY;

    // Create the RenderTarget if it does not exist
    if (PortalTexture == nullptr)
    {
        // Create new RTT
        PortalTexture = NewObject<UTextureRenderTarget2D>(
            this,
            UTextureRenderTarget2D::StaticClass(),
            *FString("PortalRenderTarget")
        );
        check( PortalTexture );

        PortalTexture->RenderTargetFormat   = ETextureRenderTargetFormat::RTF_RGBA16f;
        PortalTexture->Filter               = TextureFilter::TF_Bilinear;
        PortalTexture->SizeX                = CurrentSizeX;
        PortalTexture->SizeY                = CurrentSizeY;
        PortalTexture->ClearColor           = FLinearColor::Black;
        PortalTexture->TargetGamma          = 2.2f;
        PortalTexture->bNeedsTwoCopies      = false;
        PortalTexture->AddressX             = TextureAddress::TA_Clamp;
        PortalTexture->AddressY             = TextureAddress::TA_Clamp;
        PortalTexture->bAutoGenerateMips    = false;
        PortalTexture->UpdateResource(); // Force the engine to create the render target
    }
    else
    {
        // Resize the RenderTarget if it already exists TODO currently being unused
        PortalTexture-> ResizeTarget( CurrentSizeX, CurrentSizeY );
    }
}


