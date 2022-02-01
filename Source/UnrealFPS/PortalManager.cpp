// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"

#include "MainCharacter.h"
#include "Portal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineUtils.h"

// Sets default values
APortalManager::APortalManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    
    PortalTexture = nullptr;
    UpdateDelay = 1.1f;

    PreviousScreenSizeX = 0;
    PreviousScreenSizeY = 0;

}

// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();
}

/*
// Called every frame
void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}*/

void APortalManager::SetPlayer(AMainCharacter *NewPlayer)
{
    CharacterOwner = NewPlayer;
}

void APortalManager::Init()
{
    //------------------------------------------------
    //Create Camera
    //------------------------------------------------
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

    //------------------------------------------------
    //Create RTT Buffer
    //------------------------------------------------
    GeneratePortalTexture();
}

// Manual Tick
void APortalManager::Update(float DeltaTime)
{
    //-----------------------------------
    // Generate Portal texture ?
    //-----------------------------------
    // TODO can remove the entire section with the update delay?
    UpdateDelay += DeltaTime;

    if (UpdateDelay > 1.0f)
    {
        UpdateDelay = 0.0f;
        //GeneratePortalTexture();
    }

    //-----------------------------------
    // Find portals in the level and update them
    //-----------------------------------
    for( TActorIterator<APortal>ActorItr( GetWorld() ); ActorItr; ++ActorItr )
    {
        ActorItr->ClearRenderTargetTexture();
        UpdateCapture(*ActorItr);
    }
}

// Update SceneCapture
void APortalManager::UpdateCapture(APortal *Portal)
{
    if( CharacterOwner == nullptr )
    {
        return;
    }

    //-----------------------------------
    // Update SceneCapture (discard if there is no active portal)
    //-----------------------------------
    if (SceneCapture != nullptr && PortalTexture != nullptr && Portal != nullptr)
    {

        APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        AActor* Target  = Portal->GetTarget();

        //Place the SceneCapture to the Target
        if (Target != nullptr)
        {
            //-------------------------------
            // Compute new location in the space of the target actor
            // (which may not be aligned to world)
            //-------------------------------
            FVector NewLocation = Portal->ConvertLocationToActorSpace(PlayerCameraManager->GetCameraLocation(), Portal, Target);

            SceneCapture->SetWorldLocation(NewLocation);

            //-------------------------------
            //Compute new Rotation in the space of the target location
            //-------------------------------
            FRotator CameraRotation  = PlayerCameraManager->GetCameraRotation();
            FTransform SourceTransform  = Portal->GetActorTransform();
            FTransform TargetTransform  = Target->GetActorTransform();

            FQuat LocalQuat             = SourceTransform.GetRotation().Inverse() * CameraRotation.Quaternion();
            FQuat NewWorldQuat          = TargetTransform.GetRotation() * LocalQuat;

            //Update SceneCapture rotation
            SceneCapture->SetWorldRotation(NewWorldQuat);

            //-------------------------------
            //Clip Plane : to ignore objects between the
            //SceneCapture and the TargetPortal
            //-------------------------------
            SceneCapture->ClipPlaneNormal   = Target->GetActorForwardVector();
            SceneCapture->ClipPlaneBase     = Target->GetActorLocation() + (SceneCapture->ClipPlaneNormal * -617.5f); //Offset to avoid visible pixel border
            //UE_LOG(LogTemp, Warning, TEXT("Actor location is: %s"), *(Target->GetActorLocation()).ToString());
        }

        // Assign the Render Target
        Portal->SetRenderTargetTexture( PortalTexture );
        SceneCapture->TextureTarget = PortalTexture;

        // Get the Projection Matrix
        SceneCapture->CustomProjectionMatrix = CharacterOwner->GetCameraProjectionMatrix();

        // Say Cheeeeese !
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

    // Use a smaller size than the current
    // screen to reduce the performance impact
    CurrentSizeX = FMath::Clamp( int(CurrentSizeX / 1.7), 128, 1920); //1920 / 1.5 = 1280
    CurrentSizeY = FMath::Clamp( int(CurrentSizeY / 1.7), 128, 1080);

    if( CurrentSizeX == PreviousScreenSizeX
    &&  CurrentSizeY == PreviousScreenSizeY )
    {
        return;
    }

    PreviousScreenSizeX = CurrentSizeX;
    PreviousScreenSizeY = CurrentSizeY;


    // Create the RenderTarget if it does not exist
    if( PortalTexture == nullptr )
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

        // Not needed since the texture is displayed on screen directly
        // in some engine versions this can even lead to crashes (notably 4.24/4.25)
        PortalTexture->bAutoGenerateMips    = false;

        // This force the engine to create the render target
        // with the parameters we defined just above
        PortalTexture->UpdateResource();
    }
    // Resize the RenderTarget if it already exists
    else
    {
        // TODO we can probably remove this else case
        PortalTexture-> ResizeTarget( CurrentSizeX, CurrentSizeY );
    }
}


